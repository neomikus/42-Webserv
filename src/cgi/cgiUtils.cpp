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

bool		Request::getTimeout() {
	bool timeout;
	pthread_mutex_lock(&cgiTimeoutMutex);
	timeout = cgiTimeout;
	pthread_mutex_unlock(&cgiTimeoutMutex);
	return (timeout);
}

void		Request::setTimeout(bool value) {
	pthread_mutex_lock(&cgiTimeoutMutex);
	cgiTimeout = value;
	pthread_mutex_unlock(&cgiTimeoutMutex);
}

void		*checkTimeout(void *ptr) {
	Request *ths = (Request *)ptr;
	sleep(CGI_WAIT_TIME);
	if (!WIFEXITED(ths->getChildStatus())) {
		kill(ths->getChildPid(), SIGTERM);
		ths->setTimeout(true);
	}
	return (ptr);
}