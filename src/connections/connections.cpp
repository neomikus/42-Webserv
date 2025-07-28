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

std::string	read_request(int fd) {
	char buffer[1024];
	std::string retval;
	int rd = recv(fd, buffer, 1024, 0);
	
	if (rd == -1) {
		std::cerr << "Read failed!" << std::endl;
		return ("");
	}
	buffer[rd] = '\0';
	while (rd > 0) {
		std::cout << buffer;
		retval += buffer;
		if (rd < 1024)
			break;
		rd = recv(fd, buffer, 1024, 0);
		buffer[rd] = '\0';
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

std::string getBody(std::string &rawResponse) {
	if (rawResponse.find("\r\n") != rawResponse.npos)
		return (rawResponse.substr(0, rawResponse.size()));
	return (std::string());
}

Request *makeRequest(std::string &rawResponse)
{
	Request 			*req;
	std::stringstream	buffer;
	std::string			_temp;
	std::string			rawBody = getBody(rawResponse);
	std::vector<std::string>	splitedResponse = strSplit(rawResponse, "\n");

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

	while (!sigstop) {
		int evt_count = epoll_wait(epfd, events, 5, 200);
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
			for (int i = 0; i < evt_count; i++) {

				if (checkfds(events[i].data.fd, it->getSockets()))
					connect(epfd, events[i].data.fd, clients);

				if (checkfds(events[i].data.fd, clients)) {
					std::string rawResponse = read_request(events[i].data.fd);
					Request *request = makeRequest(rawResponse);
					request->response(events[i].data.fd, clients, request->selectServer(servers));
					delete request;
				}
			}
		}
	}
	
	for (std::list<int>::iterator it = clients.begin(); it != clients.end(); ++it) {
		send(*it, "Connection closed by server\n", strlen("Connection closed by server\n"), 0);
		close(*it);
	}
}
