#include "webserv.hpp"
#include "Post.hpp"

Post::Post(): Request() {
	contentType = "none";
}

void	Post::parseBody(std::vector<char> &rawBody) {
	// Program later
	//if (rawBody.size() > server.getMax_body_size())
	//	return ;
	std::string	resourceName;
	if (contentType == "application/x-www-form-urlencoded") {
		;
	}
	else if (contentType == "multipart/form-data") {
		
	}
	else if (contentType == "text/html") {
		;
	}

	/*
	if (!resourceName.empty())
		body.setName(resourceName);
	else
		body.setName(resource.substr(1));
	*/
	std::cout << "Copying the body..." << std::endl;
	body = rawBody;
}

Post::Post(std::vector<std::string> splitedRaw, std::vector<char> &rawBody)/*: Request(splitedRaw)*/ {
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
		if (_temp == "Content-Type:") {
			tokens >> contentType;
			tokens >> _temp;
			if (_temp.substr(0, cstrlen("boundary=")) == "boundary=") {
				_temp.substr(cstrlen("boundary="));
			}
		}
	}
	parseBody(rawBody);
}

Post::Post(const Post &model): Request(model) {

}

std::string	Post::updateResource(int &status) {
	if (status != 201 && status != 204)
		return ("");
	std::string	resourceName;
	resourceName = resource; // for now
	std::filebuf	fb;
	fb.open(resource.substr(1).c_str(), std::ios::binary | std::ios::out);
	std::ostream	newResource(&fb);

	for (std::vector<char>::iterator it = body.begin(); it != body.end(); ++it) {
		newResource.put(*it);
	}

	return (resourceName);
}

void	Post::response(int fd, std::list<int> &clients, Server &server) {
	Location 	location = selectContext(server.getVLocation(), "");
	int			status = getStatus(location);
	std::string newResource = updateResource(status);
	File		responseBody;

	getBody(status, location, responseBody);

	long long contentLenght = responseBody.getSize();

	std::string response;

	response += "HTTP/1.1 "; // This is always true
	response += toString(status);
	response += " " + getStatusText(status);
	// I don't know how much we need to add to the response?
	response += "Content Lenght: ";
	response += toString(contentLenght);
	response += "\r\n";
	
	if (status == 201) {	
		response += "Location: ";
		response += newResource;
		response += "\r\n";
	}

	response += "\r\n";

	// If the created resource is small, send it after creation (code 201)
	// If the created resource is big, send metadata (or nothing, fuck it at this point) (Code 201)
	// If !body, then code 204
	for (std::string line; std::getline(responseBody.getStream(), line);) {
		response += line;
	}

	send(fd, response.c_str(), response.length(), 0);
	close(fd);
	clients.erase(std::find(clients.begin(), clients.end(), fd));
}

Post::~Post(){}