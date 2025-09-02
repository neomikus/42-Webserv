#include "webserv.hpp"
#include "Get.hpp"

Get::Get(){}

Get::Get(std::vector<std::string> splitedResponse): Request(splitedResponse){

}

Get::Get(const Get &model): Request(model) {

}

Get::~Get(){}

bool	Get::checkRedirect(int &status) {
	if (*resource.rbegin() != '/') {
		status = 301;
		return (true);
	}
	return (false);
}

bool	Get::checkIndex(Location &location, File &responseBody) {
	std::vector<std::string> indexes = location.getIndex();
	if (indexes.empty() || !checkDirectory(resource))
		return (false);
	for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
		if (!access((resource + *it).c_str(), F_OK)) {
			responseBody.open(resource + *it);
			return (true);
		}
	}
	return (false);
}

File	generateAutoIndex(std::string directory) {
	File	retval;
	DIR		*dirPTR = opendir(directory.c_str());

	for (dirent *current = readdir(dirPTR); current; current = readdir(dirPTR)) {
		delete current;
	}
	

	closedir(dirPTR);
	return (retval);
}

bool	Get::checkAutoindex(Location &location, File &responseBody) {
	if (location.getAutoindex()) {
		responseBody = generateAutoIndex(resource);
		return (true);
	}
	return (false);
}

void	Get::getBody(int &status, Location &currentLocation, File &responseBody) {
	if (status == 200) {
		if (currentLocation.getCgi() != "")
			cgi(status, responseBody, resource, currentLocation.getCgi());
		else if (!checkDirectory(resource))
			responseBody.open(resource);
		else {
			if (checkRedirect(status))
				return;
			if (checkIndex(currentLocation, responseBody))
				return;
			if (checkAutoindex(currentLocation, responseBody))
				return;
			// Should return an error if none is true
		}
	} else if (status == 418) {
		teapotGenerator(responseBody);
	} else {
		std::string page = checkErrorPages(currentLocation.getError_pages(), status);
		if (!page.empty())
			getErrorPages(page, responseBody);
		else
			responseBody.open(DEFAULT_ERROR_PAGE);
	}
}

void	Get::response(int fd, std::list<int> &clients, Server &server) {
	Location 	location = selectContext(server.getVLocation(), "");
	int	status = getStatus(location);
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
	
	if (status == 301) {
		response += "Location: ";
		response += resource.substr(resource.find_last_of('/') + 1) + "/";
		response += "\r\n";
	}

	response += "\r\n";
	response += makeString(responseBody.getBody());

	send(fd, response.c_str(), response.size(), 0);
	close(fd);
	clients.erase(std::find(clients.begin(), clients.end(), fd));
}