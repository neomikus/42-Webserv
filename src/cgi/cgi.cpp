#include "webserv.hpp"

std::string cgi(int &status, const std::string resource, const std::string command) {
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

        // Construimos argv local para el hijo. Asegurate de que command sea ruta valida
        // O usar execvp si querés búsqueda en PATH:
        char *argv[3];
        argv[0] = const_cast<char*>(command.c_str());
        argv[1] = const_cast<char*>(file.c_str());
        argv[2] = NULL;

        // Opcional: si querés usar PATH: execvp(argv[0], argv);
        execve(argv[0], argv, global_envp);
        // Si llegamos acá, exec falló
        std::cerr << "exec failed: " << std::endl;
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