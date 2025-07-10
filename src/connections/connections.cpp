#include "webserv.hpp"
#include "Server.hpp"
#include "Request.hpp"

bool	checkfds(int fd, std::list<int> fdList) {
	if (fdList.empty())
		return (false);
	for (std::list<int>::iterator it = fdList.begin(); it != fdList.end(); it++)
	{
		if (fd == *it)
			return (true);
	}
	return (false);
}

std::string	read_request(int fd) {
	char buffer[1024];
	std::string retval;
	int rd = recv(fd, buffer, 1024, 0);
	if (rd == -1) {
		std::cerr << "Read failed!" << std::endl;
	}
	buffer[rd] = '\0';
	while (rd > 0) {
		std::cout << buffer;
		retval += buffer;
		if (rd < 1024)
			break;
		rd = recv(fd, buffer, 1024, 0);
		buffer[rd] = '\0';
	}

	return (retval);
}

void	connect(int epfd, int fd, std::list<int> &clients) {
	socklen_t	len;
	int clientfd = accept(fd, NULL, &len);
	epoll_event	clientConfig;
	clientConfig.events = EPOLLOUT;
	clientConfig.data.fd = clientfd;
	if (clientfd == -1) {
		std::cout << "Connection refused" << std::endl;
	} else {
		std::cout << "Connection accepted!" << std::endl;
		clients.push_back(clientfd);
		epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &clientConfig);
	}
}

void	acceptConnections(int epfd, std::vector<Server> &servers) {
	std::list<int>	clients;

	struct epoll_event events[5];

	while (true) {
		int evt_count = epoll_wait(epfd, events, 5, 200);
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++) {
			for (int i = 0; i < evt_count; i++) {
				if (checkfds(events[i].data.fd, it->getSockets())) {
					connect(epfd, events[i].data.fd, clients);
				}
				if (checkfds(events[i].data.fd, clients)) {
					std::string rawResponse = read_request(events[i].data.fd);
					Request request(rawResponse);
					request.response(events[i].data.fd, clients);
				}
			}
		}
	}
	
	for (std::list<int>::iterator it = clients.begin(); it != clients.end(); it++) {
		send(*it, "Connection closed by server\n", strlen("Connection closed by server\n"), 0);
		close(*it);
	}
}
