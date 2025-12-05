#include "webserv.hpp"
#include "Get.hpp"

Get::Get(): Request() {
	contentLength = 0;
	redirectUrl = "";
}

Get::Get(std::vector<std::string> splitedResponse): Request(splitedResponse){

}

Get::Get(const Get &model): Request(model) {

}

Get::~Get(){}

void	Get::parseHeader() {
	Request::parseHeader();
}

bool	Get::checkRedirect() {
	if (location.getRedirect().first) {
		status = location.getRedirect().first;
		redirectUrl = location.getRedirect().second;
	}
	if (!resource.substr(location.getRoot().size()).empty() && *resource.rbegin() != '/') {
		if (status != 302)
			status = 301;
		return (true);
	}
	if (location.getRedirect().first)
		return (true);
	return (false);
}

bool	Get::checkIndex(File &responseBody) {
	std::vector<std::string> indexes = location.getIndex();
	if (indexes.empty() || !checkDirectory(resource))
		return (false);
	for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
		if (!access((resource + *it).c_str(), F_OK)) {
			responseBody.open(resource + *it);
			return (true);
		}
	}
	return (false);
}

bool	Get::checkAutoindex(File &responseBody) {
	if (location.getAutoindex()) {
		responseBody = generateAutoIndex(resource.substr(location.getRoot().size()), resource);
		return (true);
	}
	return (false);
}

bool	Get::checkAcceptedFormats(File &responseBody) {
	std::string mime = responseBody.getType();

	if (mime == "none" || accept.empty() || std::find(accept.begin(), accept.end(), "*/*") != accept.end())
		return (false);
	if (std::find(accept.begin(), accept.end(), mime) != accept.end())
		return (false);
	if (std::find(accept.begin(), accept.end(), mime.substr(0, mime.find_last_of("/")) + "/*") != accept.end())
		return (false);
	return (true);
}

void	Get::acceptedFormats(File &responseBody) {
	if (!checkAcceptedFormats(responseBody)) {
		if (status == 200)
			status = 406;
		if (std::find(accept.begin(), accept.end(), "text/html") != accept.end() ||
			std::find(accept.begin(), accept.end(), "text/*") != accept.end())
			getBody(responseBody);
		else
			responseBody = File();
	}
}

void	Get::getBody(File &responseBody) {
	if (status == 200 || status == 301 || status == 302) {
		if (location.getCgi() != "" && !checkDirectory(resource))
			cgi();
		else if (status != 301 && status != 302 && !resource.empty() && !checkDirectory(resource))
			responseBody.open(resource);
		else {
			if (checkRedirect())
				return;
			if (checkIndex(responseBody))
				return;
			if (checkAutoindex(responseBody))
				return;
			status = 404;
			getBody(responseBody);
		}
	} else if (status == 418) {
		teapotGenerator(responseBody);
	} else {
		std::string page = checkErrorPages(location.getError_pages());
		if (!page.empty())
			getErrorPages(page, responseBody);
		else
			responseBody.open(DEFAULT_ERROR_PAGE);
	}
}
std::string Get::cgi()
{
	std::string command = location.getCgi();
	int pipefd[2];
    if (pipe(pipefd) == -1) {
        std::cerr << "pipe failed: " << std::endl;
        status = 500;
        return "";
    }

    pid_t child = fork();
    if (child == -1) {
        std::cerr << "fork failed: " << std::endl;
        close(pipefd[0]);
        close(pipefd[1]);
        status = 500;
        return "";
    }

    if (child == 0) {
        
        close(pipefd[0]);

        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            std::cerr << "dup2 stdout failed: " << std::endl;
            exit(1);
        }
        close(pipefd[1]);

        std::string file = resource;
        if (!file.empty() && file[0] == '/')
            file = file.substr(1);

		std::string env_name = "QUERY_STRING=" + query;
		const char *query_string = env_name.c_str();
		
		int env_count = 0;
		while (global_envp[env_count] != NULL)
       		env_count++;
    
    	char **new_envp = new char*[env_count + 2];
		new_envp[env_count] = new char[strlen(query_string) + 1];
		strcpy(new_envp[env_count], query_string);

		for (int i = 0; i < env_count; i++)
			new_envp[i] = global_envp[i];

    	new_envp[env_count + 1] = NULL;
		
		char *argv[3];
        argv[0] = const_cast<char*>(command.c_str());
        argv[1] = const_cast<char*>(file.c_str());
        argv[2] = const_cast<char*>(query.c_str());
        argv[3] = NULL;		

        execve(argv[0], argv, new_envp);
		
        std::cerr << "exec failed: " << std::endl;

		delete[] new_envp[env_count];
    	delete[] new_envp;
        exit(1);
    }
    else {

		int flags = fcntl(pipefd[0], F_GETFL, 0);
		fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);
        close(pipefd[1]);

        std::string output;
        char buffer[BUFFER_SIZE];
        ssize_t n;

		time_t time_start;
		time(&time_start);

        while (true)
		{
			static time_t time_running; 
			time(&time_running);
			if (time_running - time_start > CGI_WAIT_TIME)
			{
        		close(pipefd[0]);
				kill(child, SIGTERM);
				status = 504;
            	return "";
			}
			n = read(pipefd[0], buffer, sizeof(buffer));
			if (n > 0)
				output.append(buffer, n);
			if (n == 0)
				break;
		}
    
        if (n == -1)
            std::cerr << "read error: " << std::endl;

        close(pipefd[0]);

        int child_status = 0;
        if (waitpid(child, &child_status, WNOHANG) == -1) {
            std::cerr << "waitpid failed: " << std::endl;
            status = 500;
            return "";
        }
    
        if (WIFEXITED(child_status)) {
            int exit_code = WEXITSTATUS(child_status);
            if (exit_code != 0) {
                std::cerr << "child exited with code " << exit_code << std::endl;
                status = 500;
            }
            else status = 200;
    
        }
        else if (WIFSIGNALED(child_status)) {
            int sig = WTERMSIG(child_status);
            std::cerr << "child killed by signal " << sig << std::endl;
            status = 500;
        }
        else status = 500;

        return output;
    }
}

void	Get::response(int fd) {
	if (!location.getRoot().empty())
		resource = location.getRoot() + "/" + resource;
	status = getStatus();
	File		responseBody;
	std::string response;


	if (location.getCgi() != "" && !checkDirectory(resource)) {
		std::string cgi_response;
		cgi_response = cgi();
		if (status == 200) {
			
		}
		response += "HTTP/1.1 "; // This is always true
		response += toString(status);
		response += " " + getStatusText(status);
		// I don't know how much we need to add to the response?
		response += "Content Lenght: ";
		response += toString(cgi_response.size());
		response += "\r\n";
		response += cgi_response;	
	} 
	if (!(location.getCgi() != "" && !checkDirectory(resource)) || status != 200) {
		getBody(responseBody);
		acceptedFormats(responseBody);

		long long responseLenght = responseBody.getSize();

		response += "HTTP/1.1 "; // This is always true
		response += toString(status);
		response += " " + getStatusText(status);
		// I don't know how much we need to add to the response?
		response += "Content Lenght: ";
		response += toString(responseLenght);
		response += "\r\n";
	
		if (this->status == 301 || this->status == 302) {
			if (redirectUrl != "") {
				response += "Location: ";
				response += redirectUrl;
				response += "\r\n";
			} else {
				response += "Location: ";
				response += resource.substr(resource.find_last_of('/') + 1) + "/";
				response += "\r\n";
			}
		}

		if (location.getCgi() == "" || !checkDirectory(resource)) {
			response += "Content-Type: ";
			response += responseBody.getType();
			response += "\r\n";
		} else {
			response += "Content-Type: ";
			response += "text/html";
			response += "\r\n";
		}

		response += "\r\n";
		response += makeString(responseBody.getBody());
	}
	send(fd, response.c_str(), response.size(), 0);
}