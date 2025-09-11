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

int	read_request(int fd, std::vector<char> &rawResponse) {
	char buffer[BUFFER_SIZE + 1];
	int rd = recv(fd, buffer, 1024, 0);
	if (rd == -1) {
		std::cerr << "Read failed!" << std::endl;
		return (true);
	}

	for (int i = 0; i < rd; i++) {
		rawResponse.push_back(buffer[i]);
	}

	if (rd == BUFFER_SIZE)
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

Request *makeRequest(std::vector<char> &rawResponse)
{
	Request 			*req;
	std::stringstream	buffer;
	std::string			_temp;
	std::vector<char>::iterator	bodyStart;
	std::vector<char>	rawBody = getBody(rawResponse, bodyStart);
	std::string			rawHeader = makeString(rawResponse.begin(), bodyStart);
	std::vector<std::string>	splitedResponse = strSplit(rawHeader, "\n");

	buffer << splitedResponse[0];
	buffer >> _temp;
	if (_temp == "GET")
		req = new Get(splitedResponse);
	else if (_temp == "POST")
		req = new Post(splitedResponse, rawBody);
	else if (_temp == "DELETE")
		req = new Delete(splitedResponse);
	else
		req = new Request(splitedResponse);

	return (req);
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

	std::vector<char> rawResponse;
	Request *request = NULL;
	while (!sigstop)
	{
		int evt_count = epoll_wait(epfd, events, EPOLL_EVENT_COUNT, -1);
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
			for (int i = 0; i < evt_count; i++) {
				if (checkfds(events[i].data.fd, it->getSockets()))
					connect(epfd, events[i].data.fd, clients);

				if (checkfds(events[i].data.fd, clients)) {
					if ((events[i].events & EPOLLIN) && read_request(events[i].data.fd, rawResponse)) {
						request = makeRequest(rawResponse);
						handleEvent(epfd, events[i].data.fd);
					} else if (events[i].events & EPOLLOUT) {
						request->response(events[i].data.fd, request->selectServer(servers));
						rawResponse.clear();
						delete request;
						closeConnection(epfd, events[i].data.fd, clients);
					} else if (events[i].events & EPOLLHUP || events[i].events & EPOLLERR) {
						closeConnection(epfd, events[i].data.fd, clients);
						if (request)
							delete request;
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
