#include "webserv.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "File.hpp"

void	Request::parseMethodResourceProtocol(const std::string line)
{
	if (countWords(line) != 3) {
		error = true;
		return;
	}

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

	resource = ltrim(resource, '/');
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

		std::cout << *it << std::endl;

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
		if (_temp == "Content-Length:") {
			contentLength = atol(it->substr(cstrlen("Content-Length: ")).c_str());
		}
	}
}

Request::Request() {
	error = false;
	method = "";
	resource = "";
	protocol = "";
	userAgent = "";
	keepAlive = false;
	referer = "";
	contentLength = -1;
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

bool	checkAllowedMethods(std::string &method, allowed_methods methods) {
	if (method == "PUT" || method == "HEAD" || method == "CONNECT" || method == "TRACE" || method == "PATCH") // (Not Implemented Methods)
		return (false);
	if (method == "GET" && methods._get == false)
		return (false);
	if (method == "POST" && methods._post == false)
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

bool	checkPermissions(std::string resource) {
	struct stat resBuffer;

	stat(resource.c_str(), &resBuffer);

	if (!(resBuffer.st_mode & S_IRUSR))
		return (false);
	return (true);
}

int	Request::getStatus(Location &currentLocation) {
	if (error)
		return (400);
	if (method != "PUT" && method != "HEAD" &&
		method != "CONNECT" && method != "TRACE" && method != "POST" &&
		method != "PATCH" && method != "GET" && method != "DELETE")
		return (501);
	if (protocol != "HTTP/1.0" && protocol != "HTTP/1.1")
		return (505);
	if (!checkAllowedMethods(method, currentLocation.getMethods()))
		return (405);
	// Fix later
	if (false && transferEncoding != "chunked" && contentLength == -1) {
		return (411);
	}
	if (method != "POST" && !resource.empty() && access(resource.c_str(), F_OK)) {
		if (resource == "teapot")
			return (418);
		return (404);
	}
	if (!resource.empty() && !checkPermissions(resource))
		return (403);
	// Save request body size in parsing!!!
	//if (currentLocation.getMax_body_size() > request_body_size)
	//	return (413);
	if (method == "POST")
		return (201);
	return (200);
}

std::string Request::checkErrorPages(std::vector<error_page> error_pages, int &status) {
	for (std::vector<error_page>::iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
		for (std::vector<int>::iterator it2 = it->to_catch.begin(); it2 != it->to_catch.end(); ++it2) {
			if (*it2 == status) {
				if (it->to_replace > 99)
					status = it->to_replace;
				return (it->page);
			}
		}
	}
	return (DEFAULT_ERROR_PAGE);
}

void	Request::getErrorPages(std::string &page, File &responseBody) {
	if (access(page.c_str(), F_OK))
		responseBody.open(DEFAULT_ERROR_PAGE);
	else
		responseBody.open(page);
}

Location Request::selectContext(Location &location, std::string fatherUri) {
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

void	Request::getBody(int &status, Location &currentLocation, File &responseBody) {
	std::string page = checkErrorPages(currentLocation.getError_pages(), status);
	if (!page.empty())
		getErrorPages(page, responseBody);
	else
		responseBody.open(DEFAULT_ERROR_PAGE);
}

void	Request::response(int fd, Server &server) {
	Location 	location = selectContext(server.getVLocation(), "");
	if (!location.getRoot().empty())
		resource = location.getRoot() + resource;
	
	int	status = getStatus(location);

	File		responseBody;

	getBody(status, location, responseBody);

	std::string response;

	response += "HTTP/1.1 "; // This is always true
	response += toString(status);
	response += " " + getStatusText(status);
	// I don't know how much we need to add to the response?
	response += "Content Lenght: ";
	response += responseBody.getSize();
	response += "\r\n";
	
	response += "\r\n";

	response += makeString(responseBody.getBody());

	send(fd, response.c_str(), response.length(), 0);
}