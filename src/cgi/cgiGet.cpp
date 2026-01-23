#include "Get.hpp"

void Get::cgi(int epfd)
{
	std::string command = location.getCgi();
	int pipefd[2];
    if (pipe(pipefd) == -1) {
        std::cerr << "pipe failed: " << std::endl;
        status = 500;
        return;
    }
	outpipe = pipefd[0];
	epoll_event	config;

	config.events = EPOLLIN;
	config.data.fd = outpipe;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, outpipe, &config) == -1) {
		std::cerr << "Epoll failed to add pipe" << std::endl;
        status = 500;
        return;
	}

    childPid = fork();
    if (childPid == -1) {
        std::cerr << "fork failed: " << std::endl;
        close(pipefd[0]);
        close(pipefd[1]);
        status = 500;
        return;
    } else if (childPid == 0) {
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

		if (waitpid(childPid, &childStatus, WNOHANG) == -1) {
            std::cerr << "waitpid failed: " << std::endl;
            status = 502;
            return;
        }
		pthread_t timeout;
		pthread_create(&timeout, NULL, checkTimeout, (void *)this);
		pthread_detach(timeout);

		/* Child status errors
    
        if (WIFEXITED(child_status)) {
            int exit_code = WEXITSTATUS(child_status);
            if (exit_code != 0) {
                std::cerr << "child exited with code " << exit_code << std::endl;
                status = 502;
            }
            else status = 200;
        }
        else if (WIFSIGNALED(child_status)) {
            int sig = WTERMSIG(child_status);
            std::cerr << "child killed by signal " << sig << std::endl;
            status = 502;
        }
        else
			status = 502;
	
		*/
		status = 200;
	}
}

void	Get::cgiResponse(int fd, int epfd) {
	std::string og_resource = resource;
	if (!location.getRoot().empty())
		resource = location.getRoot() + "/" + resource;

	if (checkDirectory(resource))  {
		resource = og_resource;
		this->response(fd);
		return;
	}

	if (outpipe == -1) {
		cgi(epfd);
		resource = og_resource;
		if (status >= 500) {
			this->response(fd);
		}
		return;
	}

	/*if (getTimeout()) {
		resource = og_resource;
		status = 504;
		close(outpipe);
		this->response(fd);
		return;
	}*/
	
	if (pipeRead) {
		if (WEXITSTATUS(childStatus)) {
			resource = og_resource;
			this->response(fd);
			return;
		}

		if (status != 200) {
			resource = og_resource;
			this->response(fd);
			return;
		}
		std::string response;
	
		response += "HTTP/1.1 "; // This is always true
		response += toString(status);
		response += " " + getStatusText(status);
			
		response += "Content Lenght: ";
		response += toString(cgiOutput.size());
		response += "\r\n";
		response += cgiOutput;

		send(fd, response.c_str(), response.size(), 0);
		sent = true;
	}
	resource = og_resource;
}