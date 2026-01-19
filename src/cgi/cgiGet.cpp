#include "Get.hpp"

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
            status = 502;
			output.clear();
            return "";
        }
    
        if (WIFEXITED(child_status)) {
            int exit_code = WEXITSTATUS(child_status);
            if (exit_code != 0) {
                std::cerr << "child exited with code " << exit_code << std::endl;
                status = 502;
				output.clear();
            }
            else status = 200;
    
        }
        else if (WIFSIGNALED(child_status)) {
            int sig = WTERMSIG(child_status);
            std::cerr << "child killed by signal " << sig << std::endl;
            status = 502;
			output.clear();
        }
        else
		{
			status = 502;
			output.clear();
		}
        return output;
    }
}

void	Get::cgiResponse(int fd) {
	std::string og_resource = resource;
	if (!location.getRoot().empty())
		resource = location.getRoot() + "/" + resource;

	if (checkDirectory(resource))  {
		resource = og_resource;
		this->response(fd);
		return;
	}

	File		responseBody;
	std::string response;

	std::string cgi_response;
	cgi_response = cgi();
	response += "HTTP/1.1 "; // This is always true
	response += toString(status);
	response += " " + getStatusText(status);
		
	response += "Content Lenght: ";
	response += toString(cgi_response.size());
	response += "\r\n";
	response += cgi_response;

	if (status != 200) {
		response.clear();
		resource = og_resource;
		this->response(fd);
		return;
	}

	send(fd, response.c_str(), response.size(), 0);
}