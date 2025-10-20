#include "webserv.hpp"
#include "Get.hpp"

Get::Get(){}

Get::Get(std::vector<std::string> splitedResponse): Request(splitedResponse){

}

Get::Get(const Get &model): Request(model) {

}

Get::~Get(){}

bool	Get::checkRedirect(Location &location, int &status) {
	if (!resource.substr(location.getRoot().size()).empty() && *resource.rbegin() != '/') {
		status = 301;
		return (true);
	}
	return (false);
}

bool	Get::checkIndex(Location &location, File &responseBody) {
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

bool	Get::checkAutoindex(Location &location, File &responseBody) {
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

void	Get::acceptedFormats(int &status, Location &currentLocation, File &responseBody) {
	if (!checkAcceptedFormats(responseBody)) {
		if (status == 200)
			status = 406;
		if (std::find(accept.begin(), accept.end(), "text/html") != accept.end() ||
			std::find(accept.begin(), accept.end(), "text/*") != accept.end())
			getBody(status, currentLocation, responseBody);
		else
			responseBody = File();
	}
}

void	Get::getBody(int &status, Location &currentLocation, File &responseBody) {
	if (status == 200) {
		if (currentLocation.getCgi() != "" && !checkDirectory(resource))
			cgi(status, responseBody, resource, currentLocation.getCgi());
		else if (!resource.empty() && !checkDirectory(resource))
			responseBody.open(resource);
		else {
			if (checkRedirect(currentLocation, status))
				return;
			if (checkIndex(currentLocation, responseBody))
				return;
			if (checkAutoindex(currentLocation, responseBody))
				return;
			status = 404;
			getBody(status, currentLocation, responseBody);
		}
	} else if (status == 418) {
		teapotGenerator(responseBody);
	} else {
		std::string page = checkErrorPages(currentLocation.getError_pages(), status);
		if (!page.empty())
			getErrorPages(page, responseBody);
		else
			responseBody.open(DEFAULT_ERROR_PAGE);
	}
}
std::string Get::cgi(int &status, Location &location)
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
        close(pipefd[1]);

        std::string output;
        char buffer[BUFFER_SIZE];
        ssize_t n;

        while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0)
            output.append(buffer, n);
    
        if (n == -1)
            std::cerr << "read error: " << std::endl;

        close(pipefd[0]);

        int child_status = 0;
        if (waitpid(child, &child_status, 0) == -1) {
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

void	Get::response(int fd, std::list<int> &clients, Server &server) {
	Location 	location = selectContext(server.getVLocation(), "");
	if (!location.getRoot().empty())
		resource = location.getRoot() + "/" + resource;
	int	status = getStatus(location);
	File		responseBody;

	getBody(status, location, responseBody);
	acceptedFormats(status, location, responseBody);

	long long responseLenght = responseBody.getSize();

	std::string response;

	response += "HTTP/1.1 "; // This is always true
	response += toString(status);
	response += " " + getStatusText(status);
	// I don't know how much we need to add to the response?
	response += "Content Lenght: ";
	response += toString(responseLenght);
	response += "\r\n";
	
	if (status == 301) {
		response += "Location: ";
		response += resource.substr(resource.find_last_of('/') + 1) + "/" ;
		response += "\r\n";
		response += cgi_response;
	}
	else
	{
		File		responseBody;

		getBody(status, location, responseBody);

	if (location.getCgi() == "" || !checkDirectory(resource)) {
		response += "Content-Type: ";
		response += responseBody.getType();
		response += "\r\n";
	} else {
		response += "Content-Type: ";
		response += "text/html";
		response += "\r\n";
	}


		response += "HTTP/1.1 "; // This is always true
		response += toString(status);
		response += " " + getStatusText(status);
		// I don't know how much we need to add to the response?
		response += "Content Lenght: ";
		response += toString(contentLenght);
		response += "\r\n";
		
		if (status == 301) {
			response += "Location: ";
			response += resource.substr(resource.find_last_of('/') + 1) + "/" ;
			response += "\r\n";
		}

		response += "Content-Type: ";
		response += responseBody.getType();
		response += "\r\n";

		response += "\r\n";
		response += makeString(responseBody.getBody());
	}
	send(fd, response.c_str(), response.size(), 0);
}