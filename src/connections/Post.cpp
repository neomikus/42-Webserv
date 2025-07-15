#include "webserv.hpp"
#include "Post.hpp"

Post::Post(): Request() {
	contentType = "none";
}

void	Post::parseBody(std::string &rawBody) {
	// Program later
	//if (rawBody.size() > server.getMax_body_size())
	//	return ;
	(void)rawBody;
	if (contentType == "application/x-www-form-urlencoded")
		;
	else if (contentType == "application/json")
		;
	else if (contentType == "text/html")
		;
}

Post::Post(std::vector<std::string> splitedRaw, std::string &rawBody): Request(splitedRaw){
	parseMethodResourceProtocol(splitedRaw[0]);
	if (error)
		return ;
	for (std::vector<std::string>::iterator it = splitedRaw.begin(); it != splitedRaw.end(); ++it)
	{
		std::stringstream	tokens;
		tokens << *it;

		std::string _temp;
		tokens >> _temp;

		if (_temp == "Host:")
		{
			hostPort.host = it->substr(6, it->find(':', 6) - 6);
			hostPort.port = atoi(it->substr(6 + hostPort.host.length() + 1).c_str());

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
		if (_temp == "Content-Type:")
			contentType = it->substr(13);	
	}
	parseBody(rawBody);
	body << rawBody;
	std::cout << "this->host " << hostPort.host << std::endl;
	std::cout << "this->port " << hostPort.port << std::endl;
	std::cout << "this->userAgent " << userAgent << std::endl;
	std::cout << "this->accept" << std::endl;
	for (std::vector<std::string>::iterator it = accept.begin(); it != accept.end(); ++it)
		std::cout << '\t' << *it << std::endl;
	std::cout << "this->acceptEncoding" << std::endl;
	for (std::vector<std::string>::iterator it = acceptEncoding.begin(); it != acceptEncoding.end(); ++it)
		std::cout << '\t' << *it << std::endl;
	std::cout << "this->keepAlive " << keepAlive << std::endl;
	std::cout << "this->referer " << referer << std::endl;
	std::cout << "this->method " << method << std::endl;
	std::cout << "this->resource " << resource << std::endl;
	std::cout << "this->protocol " << protocol << std::endl;

	std::cout << std::endl << std::endl;
	std::cout << "this->body " << body << std::endl;
}

Post::Post(const Post &model): Request(model) {

}

std::string	Post::updateResource() {
	std::string	resourceName;
	std::filebuf fb;
	fb.open(resource.substr(1).c_str(), std::ios::out);
	resourceName = resource; // for now
	std::ostream	newResource(&fb);

	newResource << body.getStream();
	return (resourceName);
}

void	Post::response(int fd, std::list<int> &clients, Server &server) {
	Location 	location = selectContext(server.getVLocation(), "");
	std::string newResource = updateResource();
	int	status = getStatus(location);
	File		responseBody;

	std::cout << HMAG << "STATUS = " << status << std::endl;
	getBody(status, location, responseBody);

	long long contentLenght = responseBody.getSize();

	std::string response;

	response += "HTTP/1.1 "; // This is always true
	response += to_string(status);
	response += " " + getStatusText(status);
	// I don't know how much we need to add to the response?
	response += "Content Lenght: ";
	response += to_string(contentLenght);
	response += "\r\n";
	
	if (status == 201) {
		response += "Location: ";
		response += newResource;
		response += "\r\n";
	}

	response += "\r\n";

	for (std::string line; std::getline(responseBody.getStream(), line);) {
		response += line;
	}

	send(fd, response.c_str(), response.length(), 0);
	close(fd);
	clients.erase(std::find(clients.begin(), clients.end(), fd));
}

Post::~Post(){}