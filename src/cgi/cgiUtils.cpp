#include "Request.hpp"

void		Request::readFromPipe() {
	char buffer[BUFFER_SIZE + 1];
	int rd = read(outpipe, buffer, BUFFER_SIZE);
	buffer[rd] = '\0';
	
	if (rd > 0)
		cgiOutput += buffer;
}

void		Request::writeToPipe(std::vector<File> &filesVector, int epfd) {
	(void)epfd;
	std::vector<File>::iterator it = filesVector.begin();
	if (!pipeWritten)
		write(inpipe, makeString(it->getBody()).c_str(), it->getSize());
	//++it;

	//if (it == filesVector.end()) {
	pipeWritten = true;
	//}
}

void		Request::closeInpipe(int epfd) {
	close(inpipe);
	epoll_ctl(epfd, EPOLL_CTL_DEL, inpipe, NULL);
	inpipe = -2;
	pipeRead = true;
}


void		Request::closeOutpipe(int epfd) {
	close(outpipe);
	epoll_ctl(epfd, EPOLL_CTL_DEL, outpipe, NULL);
	outpipe = -2;
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
