#include "webserv.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Get.hpp"
#include "Post.hpp"
#include "Delete.hpp"


bool	checkfds(int fd, std::list<int> fdList) {
	if (fdList.empty())
		return (false);
	for (std::list<int>::iterator it = fdList.begin(); it != fdList.end(); ++it)
	{
		if (fd == *it)
			return (true);
	}
	return (false);
}

bool	checkfds(int fd, std::vector<Server> &servers) {

	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
		if (checkfds(fd, it->getSockets()))
			return (true);
	}
	return (false);
}

bool	Request::readBody(int fd) {
	if (contentLength < 0) {
		return (411);
		return (true);
	} else if (contentLength ==  0) {
		return (true);
	} else if (bodyRead == contentLength) {
		return (true);
	}

	char buffer[BUFFER_SIZE];

	int rd = recv(fd, buffer, 1024, 0);
	if (rd == -1 || rd == 0) {
		if (rd == -1)
			std::cerr << "Read error!" << std::endl;
		readError = true;
		return (false);
	}

	bodyRead += rd;

	if (error) {
		if (bodyRead == contentLength) {
			return (true);
		}
		return (false);
	}

	if (bodyRead > location.getMax_body_size()) {
		status = 413;
		error = true;
		return (false);
	}

	for (int i = 0; i < rd; i++) {
		rawBody.push_back(buffer[i]);
	}
	
	if (transferEncoding == "chunked") {
		std::string contentStr = makeString(rawBody);
		if (contentStr.find("0\r\n\r\n") != contentStr.npos)
			return (true);
		return (false);
	} else {
		if (bodyRead < contentLength) {
			return (false);
		}
		return (true);
	}
}

bool	Request::readHeader(int fd, std::vector<Server> &servers) {
	// Case = you're reading the body
	if (headerRead) {
		return (true);
	}

	std::string	alreadyRead = makeString(rawHeader);
	// You find the body in the first reading (The first line reading)
	if (alreadyRead.find("\r\n\r\n") != alreadyRead.npos) {
		for (size_t i = alreadyRead.find("\r\n\r\n") + 4; i < rawHeader.size(); i++) {
			rawBody.push_back(rawHeader[i]);
		}
		rawHeader.erase(rawHeader.begin() + alreadyRead.find("\r\n\r\n"), rawHeader.end());
		parseHeader(servers);
		bodyRead = rawBody.size();
		return (true); // Body found, stop reading header
	}

	char buffer[BUFFER_SIZE + 1];
	int rd = recv(fd, buffer, BUFFER_SIZE, 0);
	if (rd == -1 || rd == 0) {
		readError = true;
		return (true);
	}

	if (error)
		return (false);

	std::string read(buffer);

	if (read.find("\r\n\r\n") != read.npos) {
		size_t i = 0;
		for (; i < read.find("\r\n\r\n"); i++) {
			rawHeader.push_back(buffer[i]);
		}
		for (; (int)i < rd; i++) {
			rawBody.push_back(buffer[i]);
		}
		parseHeader(servers);
		return (true); // Body found, stop reading
	}


	for (int i = 0; i < rd; i++) {
		rawHeader.push_back(buffer[i]);
	}

	if (rawHeader.size() > MAX_HEADER_SIZE) {
		status = 400;
		error = true;
	}
	return (false);
}

void	connect(int epfd, int fd, std::list<int> &clients) {
	socklen_t	len;
	int clientfd = accept(fd, NULL, &len);
	epoll_event	clientConfig;
	clientConfig.events = EPOLLIN;
	clientConfig.data.fd = clientfd;
	if (clientfd == -1) {
		std::cerr << "Connection refused" << std::endl;
	} else {
		clients.push_back(clientfd);
		epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &clientConfig);
	}
}

std::vector<char> getBody(std::vector<char> &rawResponse, std::vector<char>::iterator &bodyStart) {
	std::vector<char> retval;
	for (std::vector<char>::iterator it = rawResponse.begin(); it != rawResponse.end(); ++it)
	{
		(void)bodyStart;
		if (std::distance(it, rawResponse.end()) < 4)
			break;
		if (*it == '\r' && *(it + 1) == '\n' && *(it + 2) == '\r' && *(it + 3) == '\n') {
			it += 4;
			bodyStart = it;
			for (std::vector<char>::iterator it2 = it; it2 != rawResponse.end(); ++it2) {
				retval.push_back(*it2);
			}
			break;
		}
	}
	return (retval);
}


Request *makeRequest(int fd)
{
	std::vector<char> rawRequest;
	char buffer[BUFFER_SIZE * 2];
	int rd = recv(fd, buffer, BUFFER_SIZE, 0);
	Request *retval;
	if (rd == -1 || rd == 0) {
		retval = new Request();
		if (rd == -1)
			std::cerr << "Read error!" << std::endl;
		retval->setReadError(true);
	}

	for (int i = 0; i < rd; i++) {
		rawRequest.push_back(buffer[i]);
	}

	std::string request = makeString(rawRequest);

	if (request.find("\r\n") == request.npos) {
		retval = new Request();
		retval->setStatus(400);
		return (retval);
	}
	
	request = request.substr(0, request.find("\r\n"));

	if (countWords(request) != 3) {
		retval = new Request();
		retval->setStatus(400);
		return (retval);
	}

	std::stringstream	strBuffer;

	std::string method;
	std::string resource;
	std::string protocol;

	strBuffer << request;
	strBuffer >> method;
	strBuffer >> resource;
	strBuffer >> protocol;

	if (method == "GET")
		retval = new Get();
	else if (method == "POST")
		retval = new Post();
	else if (method == "DELETE")
		retval = new Delete();
	else
		retval = new Request();

	retval->getMethod() = method;
	retval->getProtocol() = protocol;

	size_t tokenPos = resource.find("?");
	if (tokenPos != resource.npos)
		retval->getQuery() = resource.substr(tokenPos + 1);
	retval->getResource() = resource.substr(0, resource.find("?"));
	retval->getResource() = ltrim(retval->getResource(), '/');

	retval->getRawHeader() = rawRequest;
	return (retval);
}

void	handleEvent(int epfd, int evtfd) {
	epoll_event	config;

	config.events = EPOLLOUT;
	config.data.fd = evtfd;
	epoll_ctl(epfd, EPOLL_CTL_MOD, evtfd, &config);
}

void	closeConnection(int epfd, int evtfd, std::list<int> &clients) {
	epoll_ctl(epfd, EPOLL_CTL_DEL, evtfd, NULL);
	close(evtfd);
	clients.erase(std::find(clients.begin(), clients.end(), evtfd));
}

void	handleEpollEvent(int epfd, std::map<int, Request *> &requests, 	std::list<int>	&clients, std::vector<Server> &servers, int event, int fd) {
	std::vector<char> rawRequest;

	if ((event & EPOLLIN) && !requests[fd]) {
		requests[fd] = makeRequest(fd);
	}
	Request *currentRequest = requests[fd];
	if ((event & EPOLLIN)
				&& currentRequest->readHeader(fd, servers) 
				&& !currentRequest->getReadError() 
				&& currentRequest->readBody(fd)) {
		handleEvent(epfd, fd);
	} else if (event & EPOLLOUT) {
		if (currentRequest->getLocation().getCgi() != "" &&
		(currentRequest->getMethod() == "GET" || currentRequest->getMethod() == "POST"))
			currentRequest->cgiResponse(fd, epfd);
		else
			currentRequest->response(fd);
		if (currentRequest->getSent()) {
			rawRequest.clear();
			delete requests[fd];
			requests.erase(fd);
			closeConnection(epfd, fd, clients);
		}
	} if (event & EPOLLHUP || event & EPOLLERR || (requests[fd] && requests[fd]->getReadError())) {
		closeConnection(epfd, fd, clients);
		if (requests[fd])
			delete requests[fd];
		requests.erase(fd);
	}
}


void	acceptConnections(int epfd, std::vector<Server> &servers) {
	std::list<int>	clients;

	struct epoll_event events[EPOLL_EVENT_COUNT];

	// FD, Request map
	std::map<int, Request *> requests;
	std::cout << "Accepting connections..." << std::endl;
	while (!sigstop)
	{
		int evt_count = epoll_wait(epfd, events, EPOLL_EVENT_COUNT, -1);
		for (int i = 0; i < evt_count; i++) {
			if (checkfds(events[i].data.fd, servers)) {
				connect(epfd, events[i].data.fd, clients);
				requests[events[i].data.fd] = NULL;
			} else if (checkfds(events[i].data.fd, clients)) {
				handleEpollEvent(epfd, requests, clients, servers, events[i].events, events[i].data.fd);
			} else {
				/* PIPE CASE */
			}
		}
	}

	for (std::map<int, Request *>::iterator it = requests.begin(); it != requests.end(); ++it) {
		if (it->second)
			delete it->second;
	}	
	
	for (std::list<int>::iterator it = clients.begin(); it != clients.end(); ++it) {
		send(*it, "Connection closed by server\r\n", cstrlen("Connection closed by server\r\n"), 0);
		epoll_ctl(epfd, EPOLL_CTL_DEL, *it, NULL);
		close(*it);
	}
}
