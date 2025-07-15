#include "webserv.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "File.hpp"

void	Request::parseMethodResourceProtocol(const std::string line)
{
	if (countWords(line) != 3)
		error = true;

	std::stringstream	buffer;
	buffer << line;
	buffer >> method;
	buffer >> resource;
	buffer >> protocol;

	std::vector<std::string>	queries = strSplit(resource.substr(resource.find("?") + 1, resource.size()), "&");
	for (std::vector<std::string>::iterator it = queries.begin(); it != queries.end(); ++it) {
		std::vector<std::string>	currentQuery = strSplit(*it, "=");
		if (currentQuery.size() != 2)
			continue;
		std::pair<std::string, std::string>	temp(currentQuery.front(), currentQuery.back());
	}
	
	resource = resource.substr(0, resource.find("?"));
}


Request::Request(std::vector<std::string> splitedRaw) {
	*this = Request();
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
	}
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


Request	&Request::operator=(const Request &model) {
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
	return (*this);
}


Request::~Request() {
	
}

long	getBodySize(std::fstream &requestBody) {
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
		case 405:
			return ("Method Not Allowed\r\n");
		case 418:
			return ("I'm a teapot\r\n");
		case 501:
			return ("Not Implemented\r\n");
		default:
			break;
	}
	return ("\r\n");
}

bool	checkAllowedMethods(std::string &method, allowed_methods methods) {
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

Server	&Request::selectServer(std::vector<Server> &servers) {

	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
		std::vector<hostport> hostports = it->getHostports();
		for (std::vector<hostport>::iterator it2 = hostports.begin(); it2 != hostports.end(); ++it2) {
			if ((hostPort.host == it2->host || it2->host == "0.0.0.0") && (hostPort.port == it2->port || it2->port == -1))
				candidates.push_back(*it);
		}
	}

	if (candidates.empty())
		return (*servers.begin());

	for (std::vector<Server>::iterator it = candidates.begin(); it != candidates.end(); ++it) {
		if (it->getServer_name() == hostPort.host) {
			return (*it);
		}
	}
	return (*candidates.begin());
}

int	Request::getStatus(Location &currentLocation) {
	if (error)
		return (400);
	if (method != "PUT" && method != "HEAD" &&
		method != "CONNECT" && method != "TRACE" && 
		method != "PATCH" && method != "GET" && method != "DELETE")
		return (501);
	if (!checkAllowedMethods(method, currentLocation.getMethods()))
		return (405);
	if (access(resource.substr(1).c_str(), F_OK)) {
		if (resource.substr(1) == "teapot")
			return (418);
		return (404);
	}
	// Save request body size in parsing!!!
	//if (currentLocation.getMax_body_size() > request_body_size)
	//	return (413);
	if (method == "POST")
		return (201);
	return (200);
}

std::string checkErrorPages(std::vector<error_page> error_pages, int &status) {
	for (std::vector<error_page>::iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
		for (std::vector<int>::iterator it2 = it->to_catch.begin(); it2 != it->to_catch.end(); ++it2) {
			if (*it2 == status) {
				if (it->to_replace > 199)
					status = it->to_replace;
				return (it->page);
			}
		}
	}
	return ("");
}

void	Request::getErrorPages(std::string &page, File &responseBody) {
	if (access(page.c_str(), F_OK))
		responseBody.open(DEFAULT_ERROR_PAGE);
	else
		responseBody.open(page);
}

void	Request::getBody(int &status, Location &currentLocation, File &responseBody) {
	if (status == 200) {
		responseBody.open(resource.substr(1, resource.find("?") - 1));
		return;
	}
	if (status == 418) {
		teapotGenerator(responseBody);
		return;
	}
	if (status / 100 == 4 || status / 100 == 5) {
		std::string page = checkErrorPages(currentLocation.getError_pages(), status);
		if (!page.empty()) {
			getErrorPages(page, responseBody);
			return;
		}
	}
	responseBody.open(DEFAULT_ERROR_PAGE);
}

Location Request::selectContext(Location &location, std::string fatherUri) {

	std::cout << "FatherUri : " << fatherUri << std::endl;
	std::string uri = resource;
	if (uri[uri.size() - 1] == '/')
		uri.erase(uri.end() - 1);
	if (fatherUri != "/")
		uri = uri.substr(fatherUri.size());
	else
		fatherUri = "";
	std::cout << "im searching " << resource << std::endl;
	std::cout << "im in " << location.getUri() << std::endl;

	while(!uri.empty())
	{	
		for (std::vector<Location>::iterator it = location.getLocations().begin(); it != location.getLocations().end(); ++it)
		{
			std::cout  << "resource: [" << uri << "]" << " location uri: [" << it->getUri() << "]" << std::endl;
			if (it->getUri() == uri)
				return (selectContext(*it, fatherUri + it->getUri()));
		}
		if (uri == "/")
			break;
		uri = trimLastWord(uri, '/');
		std::cout << "trimed [" << uri << "]" << std::endl;
	}
	return (location);
}

void	Request::response(int fd, std::list<int> &clients, Server &server) {
	Location 	location = selectContext(server.getVLocation(), "");
	int	status = getStatus(location);
	File		responseBody;

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
	
	response += "\r\n";

	for (std::string line; std::getline(responseBody.getStream(), line);) {
		response += line;
	}

	send(fd, response.c_str(), response.length(), 0);
	close(fd);
	clients.erase(std::find(clients.begin(), clients.end(), fd));
}