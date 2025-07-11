#include "webserv.hpp"
#include "Request.hpp"
#include "Server.hpp"

void	Request::parseMethodResourceProtocol(const std::string line)
{
	if (countWords(line) != 3)
		error = true;

	std::stringstream	buffer;
	buffer << line;
	buffer >> method;
	buffer >> resource;
	buffer >> protocol;
}


Request::Request(std::vector<std::string> splitedRaw) {
	*this = Request();
	parseMethodResourceProtocol(splitedRaw[0]);
	for (std::vector<std::string>::iterator it = splitedRaw.begin(); it != splitedRaw.end(); it++)
	{
		std::stringstream	tokens;
		tokens << *it;

		std::string _temp;
		tokens >> _temp;

		if (_temp == "Host:")
		{
			hostPort.host = it->substr(7, it->find(':', 7) - 7);
			hostPort.port = atoi(it->substr(7 + hostPort.host.length() + 1).c_str());

		}
		if (_temp == "User-Agent:")
			userAgent = it->substr(12);
		if (_temp == "Accept:")
			accept = strSplit(it->substr(8), ",");
		if (_temp == "Accept-Encoding:")
			acceptEncoding = strSplit(it->substr(17), ", ");
		if (_temp == "Connection:")
		{
			tokens >> _temp;
			if (_temp == "keep-alive")
				keepAlive = true;
		}
		if (_temp == "Referer:")
			referer = it->substr(9);		
	}
	std::cout << "this->host " << hostPort.host << std::endl;
	std::cout << "this->port " << hostPort.port << std::endl;
	std::cout << "this->userAgent " << userAgent << std::endl;
	std::cout << "this->accept" << std::endl;
	for (std::vector<std::string>::iterator it = accept.begin(); it != accept.end(); it++)
		std::cout << '\t' << *it << std::endl;
	std::cout << "this->acceptEncoding" << std::endl;
	for (std::vector<std::string>::iterator it = acceptEncoding.begin(); it != acceptEncoding.end(); it++)
		std::cout << '\t' << *it << std::endl;
	std::cout << "this->keepAlive " << keepAlive << std::endl;
	std::cout << "this->referer " << referer << std::endl;
	std::cout << "this->method " << method << std::endl;
	std::cout << "this->resource " << resource << std::endl;
	std::cout << "this->protocol " << protocol << std::endl;

	
}

Request::Request() {
	error = false;
	method = "";
	resource = "";
	protocol = "";
	userAgent = "";
	keepAlive = false;
	referer = "";
}

Request::Request(const Request &model) {
	this->error = model.error;
	this->method = model.method;
	this->resource = model.resource;
	this->protocol = model.protocol;
	this->hostPort = model.hostPort;
	this->userAgent = model.userAgent;
	this->accept = model.accept;
	this->acceptEncoding = model.acceptEncoding;
	this->keepAlive = model.keepAlive;
	this->referer = model.referer;
}


Request::~Request() {
	
}

long	getBodySize(std::ifstream &requestBody) {
	std::streampos pagePos = requestBody.tellg();
	requestBody.seekg(0, std::ios::end);
	pagePos = requestBody.tellg() - pagePos;
	requestBody.seekg(std::ios::beg);
	return (static_cast<long>(pagePos));
}

std::string	getStatusText(int status) {
	switch (status)
	{
		case 200:
			return ("OK\r\n");
		case 404:
			return ("Not Found\r\n");
		case 418:
			return ("I'm a teapot\r\n");
		default:
			break;
	}
	return ("\r\n");
}

bool	checkAllowedMethods(std::string &method, allowed_methods methods) {
	return (true);
	if (method == "PUT" || method == "HEAD" || method == "CONNECT" || method == "TRACE" || method == "PATCH") // (Not Implemented Methods)
		return (false);
	if (method == "GET" && methods._get == false)
		return (false);
	if (method == "POST" && methods._delete == false)
		return (false);
	if (method == "DELETE" && methods._delete == false)
		return (false);
	return (true);
}

Server	&Request::selectServer(std::vector<Server> &servers, int fd) {
	std::vector<Server> candidates;
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++) {
		sockaddr	*temp = (sockaddr *)malloc(sizeof(sockaddr) * 1024);
		socklen_t	temp2 = 1024;
		
		getsockname(fd, temp, &temp2);
		std::cout << temp->sa_data << std::endl;
	}
	return (*servers.begin());
}

int	Request::getStatus(const Server &server) {
	if (error)
		return (400);
	if (access(resource.substr(1).c_str(), F_OK)) {
		if (resource.substr(1) == "teapot")
			return (418);
		return (404);
	}
	// Save request body size in parsing!!!
	//if (server.getMax_body_size() > request_body_size)
	//	return (413);
	if (method != "PUT" && method != "HEAD" &&
		method != "CONNECT" && method != "TRACE" && method != "PATCH" &&
		method != "GET" && method != "DELETE" && method != "PUT")
		return (501);
	if (!checkAllowedMethods(method, server.getMethods()))
		return (405);

	return (200);
}

std::vector<error_page>::iterator 	checkErrorPages(std::vector<error_page> error_pages, int &status) {
	for (std::vector<error_page>::iterator it = error_pages.begin(); it != error_pages.end(); it++) {
		for (std::vector<int>::iterator it2 = it->to_catch.begin(); it2 != it->to_catch.end(); it2++) {
			if (*it2 == status) {
				if (it->to_replace != -1)
					status = it->to_replace;
				return (it);
			}
		}
	}
	return (error_pages.end());
}

std::string	Request::getErrorPages(std::vector<error_page>::iterator error_page) {
	if (access(error_page->page.substr(1).c_str(), F_OK))
		return (DEFAULT_ERROR_PAGE);
	return (error_page->page.substr(1));
}

std::string	Request::getBody(int &status, const Server &server) {
	if (status == 200)
		return (resource.substr(1));
	//if (status == 418)
	//	return (teapotGenerator());
	if (status % 100 == 4 || status % 100 == 5) {
		std::vector<error_page>::iterator it = checkErrorPages(server.getError_pages(), status);
		if (it != server.getError_pages().end())
			return (getErrorPages(it));
	}
	return (DEFAULT_ERROR_PAGE);
}

void	Request::response(int fd, std::list<int> &clients, const Server &server) {
	int	status = getStatus(server);
	std::ifstream	responseBody(getBody(status, server).c_str());

	long contentLenght = getBodySize(responseBody);
	std::cout << contentLenght << std::endl;

	std::string response;

	response += "HTTP/1.1 "; // This is always true
	response += to_string(status);
	response += " " + getStatusText(status);
	// I don't know how much we need to add to the response?
	
	response += "Content Lenght: ";
	response += to_string(contentLenght);
	response += "\r\n";
	
	response += "\r\n";

	for (std::string line; std::getline(responseBody, line);) {
		response += line;
	}

	for (size_t i = 0; i < response.size(); i++)
	{
		std::cout << response[i];
	}
	std::cout << std::endl << "RESPONSE ENDED!" << std::endl;

	std::cout << send(fd, response.c_str(), response.length(), 0) << std::endl;
	responseBody.close();
	close(fd);
	clients.erase(std::find(clients.begin(), clients.end(), fd));

}