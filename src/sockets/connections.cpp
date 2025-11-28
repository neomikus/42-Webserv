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

bool	Request::readHeader(int fd) {
	// Case = you're reading the body
	if (headerRead) {
		return (true);
	}

	std::string	alreadyRead = makeString(rawHeader);
	// You find the body in the first reading (The first line reading)
	if (alreadyRead.find("\r\n\r\n")) {
		parseHeader();
		for (int i = alreadyRead.find("\r\n\r\n"); i < rawHeader.size(); i++) {
			rawBody.push_back(rawHeader[i]);
		}
		return (true); // Body found, stop reading
	}

	char buffer[BUFFER_SIZE + 1];
	int rd = recv(fd, buffer, 1024, 0);
	if (rd == -1) {
		std::cerr << "Read failed!" << std::endl;
		return (true);
	}

	std::string read(buffer);

	if (read.find("\r\n\r\n")) {
		int i = 0;
		for (; i < read.find("\r\n\r\n"); i++) {
			rawHeader.push_back(buffer[i]);
		}
		for (; i < rd; i++) {
			rawBody.push_back(buffer[i]);
		}
		parseHeader();
		return (true); // Body found, stop reading
	}


	for (int i = 0; i < rd; i++) {
		rawHeader.push_back(buffer[i]);
	}

	if (rawHeader.size() != MAX_HEADER_SIZE)
		return (false);
	return (true);
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
		std::cerr << "Connection accepted" << std::endl;
		clients.push_back(clientfd);
		epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &clientConfig);
	}
}

std::vector<char> getBody(std::vector<char> &rawResponse, std::vector<char>::iterator &bodyStart) {
	std::vector<char> retval;
	//for (std::vector<char>::iterator it = std::find(rawResponse.begin(), rawResponse.end(), '\r'); it != rawResponse.end(); it = std::find(it + 1, rawResponse.end(), '\r'))
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

Location	selectContext(Location &location, std::string fatherUri, std::string resource) {
   std::string uri = "/" + resource;

	if (uri[uri.size() - 1] == '/')
		uri.erase(uri.end() - 1);
	if (fatherUri != "/")
		uri = uri.substr(fatherUri.size());
	else
		fatherUri = "";

	while (!uri.empty() && uri != "/")
	{
		for (std::vector<Location>::iterator it = location.getLocations().begin(); it != location.getLocations().end(); ++it)
		{
			if (it->getUri()[0] == '*' && uri.substr(uri.size() - (it->getUri().size() - 1)) == it->getUri().substr(1))
				return (*it);
			if (it->getUri() == uri)
				return (selectContext(*it, fatherUri + it->getUri()));
		}
		uri = trimLastWord(uri, '/');
	}

	return (location);
}


Request *makeRequest(int fd, Server &server)
{
	std::vector<char> rawRequest;
	char buffer[BUFFER_SIZE * 2];
	int rd = recv(fd, buffer, 1, 0);
	Request *retval;
	
	if (rd < 0) {
		/* Devolver un error 500 si read falla */

		retval = new Request;
		return (retval);
	}

	for (int i = 0; i < rd; i++) {
		rawRequest.push_back(buffer[i]);
	}

	std::string request = makeString(rawRequest);

	if (request.find("\n") == request.npos) {
		/*  */
		return (NULL);
	}

	request = request.substr(0, request.find("\n"));

	if (countWords(request) != 3) {
		/* ERROR 400 */
		return (NULL);
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
	retval->getResource() = ltrim(resource, '/');

	retval->getRawRequest() = rawRequest;
	retval->getLocation() = selectContext(server.getVLocation(), "", retval->getResource())
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


void	acceptConnections(int epfd, std::vector<Server> &servers) {
	std::list<int>	clients;

	struct epoll_event events[EPOLL_EVENT_COUNT];


	std::vector<char> rawRequest;
	// FD, Request map
	std::map<int, Request *> requests;
	while (!sigstop)
	{
		int evt_count = epoll_wait(epfd, events, EPOLL_EVENT_COUNT, -1);
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
			for (int i = 0; i < evt_count; i++) {
				if (checkfds(events[i].data.fd, it->getSockets())) {
					connect(epfd, events[i].data.fd, clients);
					requests[events[i].data.fd] = NULL;
				}

				if (checkfds(events[i].data.fd, clients)) {
					Request	*currentRequest = requests[events[i].data.fd];
					if ((events[i].events & EPOLLIN) && !currentRequest) {
						currentRequest = makeRequest(events[i].data.fd, currentRequest->selectServer(servers));
					} else if ((events[i].events & EPOLLIN) && currentRequest->readHeader(events[i].data.fd, currentRequest)) {
						handleEvent(epfd, events[i].data.fd);
					} else if (events[i].events & EPOLLOUT) {
						currentRequest->response(events[i].data.fd, currentRequest->selectServer(servers));
						rawRequest.clear();
						delete currentRequest;
						closeConnection(epfd, events[i].data.fd, clients);
					} else if (events[i].events & EPOLLHUP || events[i].events & EPOLLERR) {
						closeConnection(epfd, events[i].data.fd, clients);
						if (currentRequest)
							delete currentRequest;
					}
				}
			}
		}
	}
	
	for (std::list<int>::iterator it = clients.begin(); it != clients.end(); ++it) {
		send(*it, "Connection closed by server\r\n", cstrlen("Connection closed by server\r\n"), 0);
		epoll_ctl(epfd, EPOLL_CTL_DEL, *it, NULL);
		close(*it);
	}
}
