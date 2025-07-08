#include "webserv.hpp"

static bool	sigstop = false;

void stop(int sig) {
	if (sig == SIGINT) {
		sigstop = true;
	}
}

bool	checkClients(int fd, std::list<int> clients) {
	if (clients.empty())
		return (false);
	for (std::list<int>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (fd == *it)
			return (true);
	}
	return (false);
}

int	main(/*int argc, char **argv*/) {
	signal(SIGINT, stop);

	int socketfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (!socketfd) {
		std::cerr << strerror(errno) << std::endl;
		return (0);
	}

	int	option = true;
	struct timeval tv;
	tv.tv_sec = 200;
	tv.tv_usec = 0;

	setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	setsockopt(socketfd, SOL_SOCKET, SO_KEEPALIVE, &option, sizeof(option));

	sockaddr_in	sockaddress;

	sockaddress.sin_family = AF_INET;
	sockaddress.sin_port = htons(PORT);
	sockaddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if (bind(socketfd, (sockaddr *)&sockaddress, sizeof(sockaddr)) < 0) {
		std::cerr << strerror(errno) << std::endl;
		close(socketfd);
		return (0);
	}

	int	epfd = epoll_create(42);
	epoll_event	config;

	config.events = EPOLLIN;
	config.data.fd = socketfd;

	if (listen(socketfd, 5) < 0) {
		std::cerr << strerror(errno) << std::endl;
		close(socketfd);
		return (0);
	}

	epoll_ctl(epfd, EPOLL_CTL_ADD, socketfd, &config);

	std::list<int>	clients;

	struct epoll_event events[5];
	while (!sigstop)
	{
		char buffer[1024];
	
		int evt_count = epoll_wait(epfd, events, 5, 200);
		socklen_t	len;
		for (int i = 0; i < evt_count; i++) {
			if (events[i].data.fd == socketfd) {
				int clientfd = accept(socketfd, (sockaddr *)&sockaddress, &len);
				epoll_event	clientConfig;
				clientConfig.events = EPOLLOUT;
				clientConfig.data.fd = clientfd;
				if (clientfd == -1) {
					std::cout << "Connection refused" << std::endl;
				} else {
					std::cout << "Connection accepted!" << std::endl;
					clients.push_back(clientfd);
					epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &clientConfig);
					//send(clientfd, "Connection established\n", strlen("Connection established\n"), 0);
				}
			}
			if (checkClients(events[i].data.fd, clients)) {				
				int rd = recv(events[i].data.fd, buffer, 1024, 0);
				if (rd == -1) {
					std::cerr << "Read failed!" << std::endl;
				}
				buffer[rd] = '\0';
				while (rd > 0) {
					std::cout << buffer;
					if (rd < 1024)
						break;
					rd = recv(events[i].data.fd, buffer, 1024, 0);
					buffer[rd] = '\0';
				}
				std::ifstream	webpage("www/error.html");

				std::streampos pagePos = webpage.tellg();
				webpage.seekg(0, std::ios::end);
				pagePos = webpage.tellg() - pagePos;
				std::stringstream contentLenght;
				contentLenght << static_cast<long>(pagePos);
				webpage.seekg(std::ios::beg);

				std::cout << contentLenght.str() << std::endl;
				std::string reply =  "HTTP/1.1 404 Not Found\r\n";
				reply += "Content-Length: " + contentLenght.str() + "\r\n";
				reply += "\r\n";

				for (std::string line; std::getline(webpage, line);) {
					reply += line;
				}

				send(events[i].data.fd, reply.c_str(), reply.length(), 0);
				webpage.close();
				close(events[i].data.fd);
				clients.erase(std::find(clients.begin(), clients.end(), events[i].data.fd));
			}
			
		}
	}

	for (std::list<int>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		send(*it, "Connection closed by server\n", strlen("Connection closed by server\n"), 0);
		close(*it);
	}

	std::cout << std::endl << "Exited cleanly!" << std::endl;
	close(socketfd);
}