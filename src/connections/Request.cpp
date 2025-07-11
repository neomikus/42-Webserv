#include "webserv.hpp"
#include "Request.hpp"

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


Request::Request(std::vector<std::string> splitedRaw): Request() {
	parseMethodResourceProtocol(splitedRaw[0]);
	for (std::vector<std::string>::iterator it = splitedRaw.begin(); it != splitedRaw.end(); it++)
	{
		std::stringstream	tokens;
		tokens << *it;

		std::string _temp;
		tokens >> _temp;

		if (_temp == "Host:")
		{
			hostport.host = it->substr(7, it->find(':', 7) - 7);
			hostport.port = atoi(it->substr(7 + hostport.host.length() + 1).c_str());

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
	std::cout << "this->host " << hostport.host << std::endl;
	std::cout << "this->port " << hostport.port << std::endl;
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
	this->hostport = model.hostport;
	this->userAgent = model.userAgent;
	this->accept = model.accept;
	this->acceptEncoding = model.acceptEncoding;
	this->keepAlive = model.keepAlive;
	this->referer = model.referer;
}


Request::~Request() {
	
}

long	getPageSize(std::ifstream &webpage) {
	std::streampos pagePos = webpage.tellg();
	webpage.seekg(0, std::ios::end);
	pagePos = webpage.tellg() - pagePos;
	webpage.seekg(std::ios::beg);
	return (static_cast<long>(pagePos));
}

std::string	getStatusText(int status) {
	switch (status)
	{
		case 200:
			return ("OK\r\n");
		case 404:
			return ("Not Found\r\n");
		default:
			break;
	}
	return ("\r\n");
}

void	Request::response(int fd, std::list<int> &clients) {
	std::ifstream	webpage(resource.substr(1).c_str());
	std::string		status;

	if (method != "GET") {
		std::cout << method << " not implemented" << std::endl;
		return;
	}

	if (!webpage) {
		status = "404";

		// Compare with server error pages, else:
		webpage.close();
		webpage.open("www/default_error_page.html");
	}
	else
		status = "200"; // Change later for different statuses

	long contentLenght = getPageSize(webpage);

	std::string reply;

	reply += "HTTP/1.1 "; // This is always true
	reply += status + " ";
	reply += getStatusText(atoi(status.c_str()));
	// I don't know how much we need to add to the response?
	
	reply += "Content Lenght: ";
	reply += contentLenght;
	reply += "\r\n";
	
	reply += "\r\n";

	for (std::string line; std::getline(webpage, line);) {
		reply += line;
	}

	send(fd, reply.c_str(), reply.length(), 0);
	webpage.close();
	close(fd);
	clients.erase(std::find(clients.begin(), clients.end(), fd));

}