#include "webserv.hpp"

std::string read_from_pipe(int fd) {
    char buffer[BUFFER_SIZE + 1];
    std::string retval;
    ssize_t rd = read(fd, buffer, sizeof(buffer));

	if (rd == -1) {
		std::cerr << "Read failed!" << std::endl;
		return ("");
	}

	buffer[rd] = '\0';
    while (rd > 0) {
		retval += buffer;
		if (rd < 1024)
			break;
		rd = read(fd, buffer, 1024);
		buffer[rd] = '\0';
    }
	
    return retval;
}

void cgi(int &status,File &responseBody, std::string resource, std::string command) {    
    int _pipe[2];
    if (pipe(_pipe) == -1) {
        std::cerr << "pipe failed" << std::endl;
        return;
    }

    pid_t child = fork();
    if (child == -1) {
        std::cerr << "fork failed" << std::endl;
        close(_pipe[0]);
        close(_pipe[1]);
        return;
    }

    if (child == 0) { 
        close(_pipe[0]); 
        dup2(_pipe[1], STDOUT_FILENO); 
        close(_pipe[1]);

		std::string file = resource;
		if (file[0] == '/')
			file = file.substr(1);
        char *argv[] = {const_cast<char*>(command.c_str()), const_cast<char*>(file.c_str()), NULL};

        execve(argv[0], argv, global_envp);
        
        std::cerr << "execve failed" << std::endl;
        exit(0);
    }

    else {
		int	childStatus;
        close(_pipe[1]); 
        waitpid(child, &childStatus, 0);
		if (childStatus != 0)
		{
			status = 500;
			return;
		}

        std::string response = read_from_pipe(_pipe[0]);
        responseBody.write(response.c_str());
        
        close(_pipe[0]); 
    }
}