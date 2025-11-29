#include "webserv.hpp"
#include "Delete.hpp"

Delete::Delete(): Request() {
	contentLength = 0;
}

Delete::Delete(std::vector<std::string> splitedResponse): Request(splitedResponse){
}

Delete::Delete(const Delete &model): Request(model) {
    
}

Delete::~Delete() {

}

void	Delete::parseHeader() {
	Request::parseHeader();
}

bool checkStat(std::string resource, int &status) {
	struct stat resBuffer;

	stat(resource.c_str(), &resBuffer);

	if (S_ISDIR(resBuffer.st_mode) || !(resBuffer.st_mode & S_IWUSR)) {
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
			responseBody.open(DEFAULT_ERROR_PAGE);
	}
}

void	Delete::response(int fd) {
	status = getStatus();
    deleteResource();
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

    if (status != 204) {
        response += "Content-Type: ";
        response += responseBody.getType();
        response += "\r\n";
    }

	response += "\r\n";

	response += makeString(responseBody.getBody());

	send(fd, response.c_str(), response.length(), 0);
	close(fd);
}