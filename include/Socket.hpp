#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "webserv.hpp"

class Socket
{
	private:
		Socket(/* args */);
		~Socket();
		Socket(const Socket &model);
		Socket	&operator=(const Socket &model);
	public:
		static int	initServer(int port, uint32_t address, int epfd);
};

#endif