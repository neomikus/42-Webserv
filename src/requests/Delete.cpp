#include "webserv.hpp"
#include "Delete.hpp"

Delete::Delete(): Request() {
	contentLength = 0;
}

Delete::Delete(const Delete &model): Request(model) {
    
}

Delete::~Delete() {

}

void	Delete::parseHeader(std::vector<Server> &servers) {
	Request::parseHeader(servers);
}

bool checkStat(std::string &resource, int &status) {
	struct stat resBuffer;

	if (stat(resource.c_str(), &resBuffer)) {
		status = 500;
		return (false);
	}

	if (resBuffer.st_mode && S_ISDIR(resBuffer.st_mode)) {
		status = 403;
        return (false);
	}

	if (resBuffer.st_mode && !(resBuffer.st_mode & S_IWUSR)) {
		status = 403;
		return (false);
	}

	return (true);
}

void	Delete::deleteResource() {
    if (status != 200)
        return;

    if (!checkStat(resource, status)) {
        return;
    }

    if (remove(resource.c_str())) {
        status = 500;
    }
}

void    Delete::getBody(File &responseBody) {
	if (status != 200 && status != 201) {
		std::string page = checkErrorPages(location.getError_pages());
		if (!page.empty())
			getErrorPages(page, responseBody);
		else
			errorPageGenerator(responseBody, status);
	}
}

void	Delete::response(int fd) {
	resource = location.getRoot() + "/" + resource;
	status = getStatus();
    deleteResource();
	File		responseBody;

	getBody(responseBody);
	
    std::string response;

	response += "HTTP/1.1 "; // This is always true
	response += toString(status);
	response += " " + getStatusText(status);
	 
	response += "Content Lenght: ";
    response += responseBody.getSize();
	response += "\r\n";

    if (status != 204) {
        response += "Content-Type: ";
        response += responseBody.getType();
        response += "\r\n";
    }

	response += "\r\n";

	response += makeString(responseBody.getBody());

	send(fd, response.c_str(), response.length(), 0);
	sent = true;
}