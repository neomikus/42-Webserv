#include "Socket.hpp"

void	createSockAddress(sockaddr_in &sockaddress, int port, uint32_t address) {
	sockaddress.sin_family = AF_INET;
	sockaddress.sin_port = htons(port);
	sockaddress.sin_addr.s_addr = htonl(address);
}

void	setSocketOptions(int socketfd) {
	int	option = 1;
	struct timeval tv;
	tv.tv_sec = 200;
	tv.tv_usec = 0;

	setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	setsockopt(socketfd, SOL_SOCKET, SO_KEEPALIVE, &option, sizeof(option));
}


int	Socket::initServer(int port, uint32_t address, int epfd) {
	int socketfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (!socketfd) {
		std::cerr << strerror(errno) << std::endl;
		return (-1);
	}

	setSocketOptions(socketfd);

	sockaddr_in	sockaddress;

	createSockAddress(sockaddress, port, address);

	if (bind(socketfd, (sockaddr *)&sockaddress, sizeof(sockaddr)) < 0) {
		std::cerr << strerror(errno) << std::endl;
		close(socketfd);
		return (-1);
	}

	epoll_event	config;

	config.events = EPOLLIN;
	config.data.fd = socketfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, socketfd, &config);

	if (listen(socketfd, 5) < 0) {
		std::cerr << strerror(errno) << std::endl;
		close(socketfd);
		return (-1);
	}

	return (socketfd);
}