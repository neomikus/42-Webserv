#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "webserv.hpp"
class Socket
{
	private:
		Socket();
		~Socket();
		Socket(const Socket &model);
		Socket	&operator=(const Socket &model);
	public:
		static int	initServer(int port, const char *hostname, int epfd);
};

#endif