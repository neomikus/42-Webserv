#include "webserv.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "File.hpp"
#include "Post.hpp"

Location	selectContext(Location &location, std::string fatherUri, std::string &resource) {
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
				return (selectContext(*it, fatherUri + it->getUri(), resource));
		}
		uri = trimLastWord(uri, '/');
	}

	return (location);
}

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

void	Request::parseHeader(std::vector<Server> &servers) {
	std::vector<std::string>	header = strSplit(makeString(rawHeader), "\r\n");

	for (std::vector<std::string>::iterator it = header.begin(); it != header.end(); ++it)
	{
		if (it->find("Host") != it->npos) {
			std::string temp = strTrim(it->substr(cstrlen("Host:")));
			hostPort.host = temp.substr(0, temp.find(":"));
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
	location = selectContext(selectServer(servers).getVLocation(), "", resource);
}

Request::Request() {
	readError = false;
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
	bodyRead = false;
	sent = false;
	inpipe = -1;
	outpipe = -1;
	pipeRead = false;
	cgiTimeout = false;
	pthread_mutex_init(&cgiTimeoutMutex, NULL);
}

Request::Request(const Request &model) {
	this->readError = model.readError;
	this->error = model.error;
	this->method = model.method;
	this->resource = model.resource;
	this->protocol = model.protocol;
	this->hostPort = model.hostPort;
	this->userAgent = model.userAgent;
	this->accept = model.accept;
	this->keepAlive = model.keepAlive;
	this->referer = model.referer;
	this->inpipe = model.inpipe;
	this->outpipe = model.outpipe;
	this->sent = model.sent;
}

Request	&Request::operator=(const Request &model) {
	this->readError = model.readError;
	this->error = model.error;
	this->method = model.method;
	this->resource = model.resource;
	this->protocol = model.protocol;
	this->hostPort = model.hostPort;
	this->userAgent = model.userAgent;
	this->accept = model.accept;
	this->keepAlive = model.keepAlive;
	this->referer = model.referer;
	this->inpipe = model.inpipe;
	this->outpipe = model.outpipe;
	this->sent = model.sent;
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
			if ((hostPort.port == it2->port || it2->port == -1))
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
	if (location.getRedirect().first) {
		return (location.getRedirect().first);
	}
	if (method != "POST" && !resource.empty() && access(resource.c_str(), F_OK)) {
		std::vector<std::string> trimmedResource = strSplit(resource, "/");
		if (trimmedResource.back() == "teapot")
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
	return ("");
}

void	Request::getErrorPages(std::string &page, File &responseBody) {
	if (access(page.c_str(), F_OK))
		errorPageGenerator(responseBody, status);
	else
		responseBody.open(page);
}

void	Request::getBody(File &responseBody) {
	std::string page = checkErrorPages(location.getError_pages());
	if (!page.empty())
		getErrorPages(page, responseBody);
	else
		errorPageGenerator(responseBody, status);
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
	 
	response += "Content Lenght: ";
	response += responseBody.getSize();
	response += "\r\n";
	
	response += "\r\n";

	response += makeString(responseBody.getBody());

	send(fd, response.c_str(), response.length(), 0);
	sent = true;
}

bool				Request::getReadError() {return(this->readError);}
bool				Request::getSent() {return(this->sent);};
void				Request::setReadError(bool value) {this->readError = value;}
std::string			&Request::getMethod() {return(this->method);}
std::string			&Request::getResource() {return(this->resource);}
std::string			&Request::getProtocol() {return(this->protocol);}
std::string			&Request::getQuery() {return(this->query);}
std::vector<char>	&Request::getRawHeader() {return(this->rawHeader);}
Location			&Request::getLocation() {return(this->location);};
int					Request::getOutpipe(){return(this->outpipe);};
void				Request::setStatus(int newStatus) {status = newStatus;};
pid_t				Request::getChildPid(){return(this->childPid);};
int					Request::getChildStatus(){return(this->childStatus);};
void				Request::cgiResponse(int fd, int epfd) {(void)fd;(void)epfd;};