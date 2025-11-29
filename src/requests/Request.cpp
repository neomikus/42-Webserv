#include "webserv.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "File.hpp"
#include "Post.hpp"

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

	size_t tokenPos = resource.find("?");
	if (tokenPos != resource.npos)
		query = resource.substr(tokenPos + 1);
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
		if (it->find("Host") != it->npos) {
			hostPort.host = strTrim(it->substr(0, it->find(':')));
			hostPort.port = atoi(it->substr(6 + hostPort.host.length() + 1).c_str());
		}
		if (it->find("User-Agent") != it->npos)
			userAgent = strTrim(it->substr(cstrlen("User-Agent:")));
		if (it->find("Accept") != it->npos)
			accept = strSplit(strTrim(it->substr(cstrlen("Accept:"))), ",");
		if (it->find("Connection") != it->npos) {
			if (strTrim(it->substr(cstrlen("Connection:"))) == "keep-alive")
				keepAlive = true;
		}
		if (it->find("Referer") != it->npos)
			referer = strTrim(it->substr(cstrlen("Referer")));
		if (it->find("Content-Lenght") != it->npos) {
			contentLength = atol(strTrim(it->substr(cstrlen("Content-Length:"))).c_str());
		}
		if (it->find("Transfer-Encoding") != it->npos) {
			transferEncoding = strTrim(it->substr(cstrlen("Transfer-Encoding:")));
		}
	}
}

void	Request::parseHeader() {
	std::vector<std::string>	header = strSplit(makeString(rawHeader), "\r\n");

	for (std::vector<std::string>::iterator it = header.begin(); it != header.end(); ++it)
	{
		if (it->find("Host") != it->npos) {
			hostPort.host = strTrim(it->substr(0, it->find(':')));
			hostPort.port = atoi(it->substr(6 + hostPort.host.length() + 1).c_str());
		} else if (it->find("User-Agent") != it->npos) {
			userAgent = strTrim(it->substr(cstrlen("User-Agent:")));
		} else if (it->find("Accept") != it->npos) {
			accept = strSplit(strTrim(it->substr(cstrlen("Accept:"))), ",");
		} else if (it->find("Connection") != it->npos) {
			if (strTrim(it->substr(cstrlen("Connection:"))) == "keep-alive")
				keepAlive = true;
		} else if (it->find("Referer") != it->npos) {
			referer = strTrim(it->substr(cstrlen("Referer")));
		} else if (it->find("Content-Length") != it->npos) {
			contentLength = atol(strTrim(it->substr(cstrlen("Content-Length: "))).c_str());
		} else if (it->find("Transfer-Encoding") != it->npos) {
			transferEncoding = strTrim(it->substr(cstrlen("Transfer-Encoding:")));
		}
	}
	headerRead = true;
}

Request::Request() {
	error = false;
	method = "";
	resource = "";
	protocol = "";
	userAgent = "";
	keepAlive = false;
	referer = "";
	transferEncoding = "";
	contentLength = -1;
	headerRead = false;
	status = 0;
}

Request::Request(const Request &model) {
	this->error = model.error;
	this->method = model.method;
	this->resource = model.resource;
	this->protocol = model.protocol;
	this->hostPort = model.hostPort;
	this->userAgent = model.userAgent;
	this->accept = model.accept;
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

bool	Request::checkPermissions() {
	struct stat resBuffer;

	stat(resource.c_str(), &resBuffer);

	if (!(resBuffer.st_mode & S_IRUSR))
		return (false);
	return (true);
}

int	Request::getStatus() {
	if (error)
		return (400);
	if (method != "PUT" && method != "HEAD" &&
		method != "CONNECT" && method != "TRACE" && method != "POST" &&
		method != "PATCH" && method != "GET" && method != "DELETE")
		return (501);
	if (protocol != "HTTP/1.0" && protocol != "HTTP/1.1")
		return (505);
	if (!checkAllowedMethods(method, location.getMethods()))
		return (405);
	if (method == "POST" && transferEncoding != "chunked" && contentLength == -1) {
		return (411);
	}
	if (method != "POST" && !resource.empty() && access(resource.c_str(), F_OK)) {
		if (resource == "teapot")
			return (418);
		return (404);
	}
	if (method != "POST" && !resource.empty() && !checkPermissions())
		return (403);
	if (method == "POST")
		return (201);
	return (200);
}

std::string Request::checkErrorPages(std::vector<error_page> error_pages) {
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

void	Request::getBody(File &responseBody) {
	std::string page = checkErrorPages(location.getError_pages());
	if (!page.empty())
		getErrorPages(page, responseBody);
	else
		responseBody.open(DEFAULT_ERROR_PAGE);
}

void	Request::response(int fd) {
	if (!location.getRoot().empty())
		resource = location.getRoot() + resource;
	
	if (!status)
		status = getStatus();

	File		responseBody;

	getBody(responseBody);

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

std::string			&Request::getMethod() {return(this->method);}
std::string			&Request::getResource() {return(this->resource);}
std::string			&Request::getProtocol() {return(this->protocol);}
std::string			&Request::getQuery() {return(this->query);}
std::vector<char>	&Request::getRawHeader() {return(this->rawHeader);}
Location			&Request::getLocation() {return(this->location);};
void				Request::setStatus(int newStatus) {status = newStatus;};
