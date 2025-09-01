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

int	Request::getStatus(Location &currentLocation) {
	if (error)
		return (400);
	if (method != "PUT" && method != "HEAD" &&
		method != "CONNECT" && method != "TRACE" && method != "POST" &&
		method != "PATCH" && method != "GET" && method != "DELETE")
		return (501);
	if (!checkAllowedMethods(method, currentLocation.getMethods()))
		return (405);
	if (method != "POST" && access(resource.substr(1).c_str(), F_OK)) {
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

std::string read_from_pipe(int fd) {
    char buffer[BUFFER_SIZE + 1];
    std::string retval;
    ssize_t rd = read(fd, buffer, sizeof(buffer));

	if (rd == -1) {
		std::cerr << "Read failed!" << std::endl;
		return ("");
	}

	buffer[rd] = '\0';
    while (rd > 0) {
		retval += buffer;
		if (rd < 1024)
			break;
		rd = read(fd, buffer, 1024);
		buffer[rd] = '\0';
    }
	
    return retval;
}

void cgi(int &status,File &responseBody, std::string resource, std::string command) {    
    int _pipe[2];
    if (pipe(_pipe) == -1) {
        std::cerr << "pipe failed" << std::endl;
        return;
    }

    pid_t child = fork();
    if (child == -1) {
        std::cerr << "fork failed" << std::endl;
        close(_pipe[0]);
        close(_pipe[1]);
        return;
    }

    if (child == 0) { 
        close(_pipe[0]); 
        dup2(_pipe[1], STDOUT_FILENO); 
        close(_pipe[1]);

		std::string file = resource;
		if (file[0] == '/')
			file = file.substr(1);
        char *argv[] = {const_cast<char*>(command.c_str()), const_cast<char*>(file.c_str()), NULL};

        execve(argv[0], argv, global_envp);
        
        std::cerr << "execve failed" << std::endl;
        exit(0);
    }

    else {
		int	childStatus;
        close(_pipe[1]); 
        waitpid(child, &childStatus, 0);
		if (childStatus != 0)
		{
			status = 500;
			return;
		}

        std::string response = read_from_pipe(_pipe[0]);
        responseBody.write(response.c_str());
        
        close(_pipe[0]); 
    }
}

void	makeForbiddenError(File &responseBody) {

	std::string body = "{\n";
	body += "\"error\": \"insufficient permissions\",";
	body += "\"message\": \"File permission error\"\n";
	body += "}\n";
	responseBody.write(body.c_str());
}

void	Request::getBody(int &status, Location &currentLocation, File &responseBody) {
	
	if (status == 200) {
		if (currentLocation.getCgi() != "")
			cgi(status, responseBody, resource, currentLocation.getCgi());
		else
			responseBody.open(resource.substr(1, resource.find("?") - 1));
		return;
	} else if (status == 201) {
		writeContent(responseBody);
	} else if (status == 204) {
		; // Nothing to return!!!
	} else if (status == 418) {
		teapotGenerator(responseBody);
	} else if (status / 100 == 4 || status / 100 == 5) {
		std::string page = checkErrorPages(currentLocation.getError_pages(), status);
		if (!page.empty()) {
			getErrorPages(page, responseBody);
		} else if (status == 403) {
			std::cout << "I'm here!!!" << std::endl;
			makeForbiddenError(responseBody);
		}
	} else {
		std::cout << "Shouldn't be here!!!" << std::endl;
		responseBody.open(DEFAULT_ERROR_PAGE);
	}
}

Location Request::selectContext(Location &location, std::string fatherUri) {
   std::string uri = resource;

   if (uri[uri.size() - 1] == '/')
	   uri.erase(uri.end() - 1);
   if (fatherUri != "/")
	   uri = uri.substr(fatherUri.size());
   else
	   fatherUri = "";

   while(!uri.empty())
   {	
	   for (std::vector<Location>::iterator it = location.getLocations().begin(); it != location.getLocations().end(); ++it)
	   {
		   if (it->getUri()[0] == '*' && uri.substr(uri.size() - (it->getUri().size() - 1)) == it->getUri().substr(1))
			   return (*it);
		   if (it->getUri() == uri)
			   return (selectContext(*it, fatherUri + it->getUri()));
	   }
	   if (uri == "/")
		   break;
	   uri = trimLastWord(uri, '/');
   }

   return (location);
}

void	Request::response(int fd, std::list<int> &clients, Server &server) {
	Location 	location = selectContext(server.getVLocation(), "");
	int			status = getStatus(location);

	std::string response;

	response += "HTTP/1.1 "; // This is always true
	response += toString(status);
	response += " " + getStatusText(status);
	// I don't know how much we need to add to the response?
	response += "Content Lenght: 0";
	response += "\r\n";
	
	response += "\r\n";

	send(fd, response.c_str(), response.length(), 0);
	close(fd);
	(void)clients;
	//clients.erase(std::find(clients.begin(), clients.end(), fd));
}