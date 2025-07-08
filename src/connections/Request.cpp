#include "webserv.hpp"
#include "Request.hpp"

Request::Request(std::string &raw) {
	method = "GET";	
	resource = raw.substr(raw.find("/") + 1, raw.find(" HTTP") - raw.find("/") - 1);
	std::cout << resource << std::endl;
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
	std::ifstream	webpage(resource.c_str());
	std::string		status;

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