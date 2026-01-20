#include "Request.hpp"

void		Request::readFromPipe() {
	char buffer[BUFFER_SIZE + 1];
	int rd = read(outpipe, buffer, BUFFER_SIZE);
	buffer[rd] = '\0';
	
	if (rd > 0)
		cgiOutput += buffer;
}

void		Request::closePipe(int epfd) {
	close(outpipe);
	epoll_ctl(epfd, EPOLL_CTL_DEL, outpipe, NULL);
	pipeRead = true;
}