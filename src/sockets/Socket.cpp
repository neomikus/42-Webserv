#include "Socket.hpp"

void	setSocketOptions(int socketfd) {
	int	option = 1;
	struct timeval tv;
	tv.tv_sec = 200;
	tv.tv_usec = 0;

	setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	setsockopt(socketfd, SOL_SOCKET, SO_KEEPALIVE, &option, sizeof(option));
}


int	Socket::initServer(int port, const char *hostname, int epfd) {
	struct addrinfo *addresses;
	struct addrinfo hints;
	hints.ai_family = AF_INET;    /* Allow IPv4 only */
	hints.ai_socktype = SOCK_STREAM; /* Use only TCP sockets (HTTP/1.1 is TCP based) h*/
	hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
	hints.ai_protocol = 0;          /* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	memset(&hints, 0, sizeof(hints));
	if (getaddrinfo(hostname, toString(port).c_str(), &hints, &addresses)) {
		std::cerr << "Can't resolve host into addresses" << std::endl;
	}

	int socketfd = -1;
	for (struct addrinfo *rp = addresses; rp != NULL; rp = rp->ai_next) {
		int sfd = socket(rp->ai_family, SOCK_STREAM | SOCK_NONBLOCK, rp->ai_protocol);
		if (sfd == -1) {
			std::cerr << strerror(errno) << std::endl;
			return (-1);
		}

		setSocketOptions(sfd);

		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) < 0) {
			close(sfd);
			continue;
		}

		socketfd = sfd;
		break;
	}

	freeaddrinfo(addresses);

	if (socketfd == -1) {
		std::cerr << "Could not bind " << hostname << ":" << port << "to any interface" << std::endl;
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

Socket::~Socket() {
	
}