#include "webserv.hpp"

#define PORT 8080

static bool	sigstop = false;

void stop(int sig) {
	if (sig == SIGINT) {
		sigstop = true;
	}
}

int	main(/*int argc, char **argv*/) {
	signal(SIGINT, stop);

	int socketfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); 

	int	option = 1;

	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR | SO_KEEPALIVE, &option, sizeof(option));

	sockaddr_in	sockaddress;

	sockaddress.sin_family = AF_INET;
	sockaddress.sin_port = htons(PORT);
	sockaddress.sin_addr.s_addr = INADDR_LOOPBACK;

	if (bind(socketfd, (sockaddr *)&sockaddress, sizeof(sockaddr)) < 0) {
		perror(strerror(errno));
		close(socketfd);
		return (0);
	}

	int	epfd = epoll_create(1);
	epoll_event	config;

	config.events = EPOLLIN;
	config.data.fd = socketfd;

	// Should I actually use listen???
	listen(socketfd, 5);

	struct epoll_event events[5];
	while (!sigstop)
	{
		char buffer[1024];
		
		int evt_count = epoll_wait(epfd, events, 5, 3000);
		socklen_t	len;
		for (int i = 0; i < evt_count; i++) {
			if (events[i].data.fd == socketfd) {
				int clientfd = accept(socketfd, (sockaddr *)&sockaddress, &len);
				if (clientfd == -1) {
					std::cout << "Connection refused" << std::endl;
				} else {
					std::cout << "Connection accepted!" << std::endl;
					epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, NULL);
					send(clientfd, "Connection established", strlen("Connection established"), 0);
					int rd = recv(clientfd, buffer, 1024, 0);
					if (rd == -1) {
						std::cerr << "Read failed!" << std::endl;
					}
					buffer[rd] = '\0';
					while (rd > 0) {
						std::cout << buffer << std::endl;
						rd = recv(clientfd, buffer, 1024, 0);
						buffer[rd] = '\0';
					}
				}
			}
		}
	}
	

	std::cout << "Exited cleanly!" << std::endl;
	close(socketfd);
}