#include "webserv.hpp"

struct pollfd {
	int   fd;         /* file descriptor */
	short events;     /* requested events */
	short revents;    /* returned events */
};


int	main(/*int argc, char **argv*/) {
	int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);


	//write(fd, );
}