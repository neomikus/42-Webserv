#include "webserv.hpp"
#include "Delete.hpp"

Delete::Delete(){}

Delete::Delete(std::vector<std::string> splitedResponse): Request(splitedResponse){
}

Delete::Delete(const Delete &model): Request(model) {
    
}

Delete::~Delete() {

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

void	Delete::deleteResource(int &status) {
    if (status != 200)
        return;

    if (!checkStat(resource, status)) {
        return;
    }

    if (remove(resource.c_str())) {
        status = 500;
    }
}

void    Delete::getBody(int &status, Location &currentLocation, File &responseBody) {
	(void)currentLocation;
	if (status == 200) {
		;
	} else if (status == 204) {
		; // Nothing to return!!!
	} else {
		std::string page = checkErrorPages(currentLocation.getError_pages(), status);
		if (!page.empty())
			getErrorPages(page, responseBody);
		else
			responseBody.open(DEFAULT_ERROR_PAGE);
	}
}

void	Delete::response(int fd, std::list<int> &clients, Server &server) {
	Location 	location = selectContext(server.getVLocation(), "");
	int	status = getStatus(location);
    deleteResource(status);
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

    if (status != 204) {
        response += "Content-Type: ";
        response += responseBody.getType();
        response += "\r\n";
    }

	response += "\r\n";

	response += makeString(responseBody.getBody());

	send(fd, response.c_str(), response.length(), 0);
	close(fd);
	(void)clients;
}