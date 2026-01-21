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
	cgiTimeoutMutex.lock();
	timeout = cgiTimeout;
	cgiTimeoutMutex.unlock();
	return (timeout);
}

void		Request::setTimeout(bool value) {
	cgiTimeoutMutex.lock();
	cgiTimeout = value;
	cgiTimeoutMutex.unlock();
}

void		checkTimeout(Request *ths, int time_start, int pid, int child_status) {
	sleep(CGI_WAIT_TIME);
	if (!WIFEXITED(child_status)) {
		ths->setTimeout(true);
	}

	/* This can be more efficient
	while (true) {
		static time_t time_running; 
		time(&time_running);
		if (time_running - time_start > CGI_WAIT_TIME) {
			kill(pid, SIGTERM);
			ths->setTimeout(true);
			return;
		} else if (WIFEXITED(child_status)) {
			return;
		}
		sleep(1);
	}
	*/
}