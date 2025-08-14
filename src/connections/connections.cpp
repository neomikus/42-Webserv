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

std::vector<char>	read_request(int fd) {
	char buffer[1024];
	std::vector<char> retval;
	int rd = recv(fd, buffer, 1024, 0);
	
	if (rd == -1) {
		std::cerr << "Read failed!" << std::endl;
		return (retval);
	}
	while (rd > 0) {
		//std::cout << buffer;
		for (int i = 0; i < rd; i++) {
			retval.push_back(buffer[i]);
		}
		rd = recv(fd, buffer, 1024, MSG_DONTWAIT);
	}
	std::cout << std::endl << std::endl;
	return (retval);
}

void	connect(int epfd, int fd, std::list<int> &clients) {
	socklen_t	len;
	int clientfd = accept(fd, NULL, &len);
	epoll_event	clientConfig;
	clientConfig.events = EPOLLOUT;
	clientConfig.data.fd = clientfd;
	if (clientfd == -1) {
		std::cout << "Connection refused" << std::endl;
	} else {
		std::cout << "Connection accepted!" << std::endl;
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
	std::cout << rawBody.size() << std::endl;
	std::string			rawHeader = makeString(rawResponse.begin(), bodyStart);
	std::vector<std::string>	splitedResponse = strSplit(rawHeader, "\n");

	buffer << splitedResponse[0];
	buffer >> _temp;
	if (_temp == "GET")
	{
		req = new Get(splitedResponse);
		std::cout << HMAG;
	}
	else if (_temp == "POST")
	{
		req = new Post(splitedResponse, rawBody);
		std::cout << HGRE;
		
	}
	else if (_temp == "DELETE")
	{
		req = new Delete(splitedResponse);
		std::cout << HRED;
	}	
	else
		req = new Request(splitedResponse);

	return (req);
}

void	acceptConnections(int epfd, std::vector<Server> &servers) {
	std::list<int>	clients;

	struct epoll_event events[5];

	while (!sigstop)
	{
		int evt_count = epoll_wait(epfd, events, 5, 200);
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
			for (int i = 0; i < evt_count; i++) {

				if (checkfds(events[i].data.fd, it->getSockets()))
					connect(epfd, events[i].data.fd, clients);

				if (checkfds(events[i].data.fd, clients)) {
					std::vector<char> rawResponse = read_request(events[i].data.fd);
					Request *request = makeRequest(rawResponse);
					request->response(events[i].data.fd, clients, request->selectServer(servers));
					delete request;
				}
			}
		}
	}
	
	for (std::list<int>::iterator it = clients.begin(); it != clients.end(); ++it) {
		send(*it, "Connection closed by server\n", cstrlen("Connection closed by server\n"), 0);
		close(*it);
	}
}
