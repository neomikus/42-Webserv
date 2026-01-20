#include "webserv.hpp"
#include "Get.hpp"

Get::Get(): Request() {
	contentLength = 0;
	redirectUrl = "";
}

Get::Get(const Get &model): Request(model) {

}

Get::~Get(){}

void	Get::parseHeader(std::vector<Server> &servers) {
	Request::parseHeader(servers);
}

bool	Get::checkRedirect() {
	if (location.getRedirect().first) {
		status = location.getRedirect().first;
		redirectUrl = location.getRedirect().second;
	}
	if (!resource.substr(location.getRoot().size()).empty() && *resource.rbegin() != '/') {
		if (status != 302)
			status = 301;
		return (true);
	}
	if (location.getRedirect().first)
		return (true);
	return (false);
}

bool	Get::checkIndex(File &responseBody) {
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

bool	Get::checkAutoindex(File &responseBody) {
	if (location.getAutoindex()) {
		responseBody = generateAutoIndex(resource.substr(location.getRoot().size()), resource);
		return (true);
	}
	return (false);
}

bool	Get::checkAcceptedFormats(File &responseBody) {
	std::string mime = responseBody.getType();

	if (mime == "none" || accept.empty() || std::find(accept.begin(), accept.end(), "*/*") != accept.end())
		return (false);
	if (std::find(accept.begin(), accept.end(), mime) != accept.end())
		return (false);
	if (std::find(accept.begin(), accept.end(), mime.substr(0, mime.find_last_of("/")) + "/*") != accept.end())
		return (false);
	return (true);
}

void	Get::acceptedFormats(File &responseBody) {
	if (!checkAcceptedFormats(responseBody)) {
		if (status == 200)
			status = 406;
		if (std::find(accept.begin(), accept.end(), "text/html") != accept.end() ||
			std::find(accept.begin(), accept.end(), "text/*") != accept.end())
			getBody(responseBody);
		else
			responseBody = File();
	}
}

void	Get::getBody(File &responseBody) {
	if (status == 200 || status == 301 || status == 302) {
		if (location.getCgi() != "" && !checkDirectory(resource))
			cgi();
		else if (status != 301 && status != 302 && !resource.empty() && !checkDirectory(resource))
			responseBody.open(resource);
		else {
			if (checkRedirect())
				return;
			if (checkIndex(responseBody))
				return;
			if (checkAutoindex(responseBody))
				return;
			status = 404;
			getBody(responseBody);
		}
	} else if (status == 418) {
		teapotGenerator(responseBody);
	} else {
		std::string page = checkErrorPages(location.getError_pages());
		if (page != "")
			getErrorPages(page, responseBody);
		else {
			errorPageGenerator(responseBody, status);
		}
	}
}

void	Get::response(int fd) {
	if (!location.getRoot().empty())
		resource = location.getRoot() + "/" + resource;
	if (!status)
		status = getStatus();
	File		responseBody;
	std::string response;

	getBody(responseBody);
	acceptedFormats(responseBody);

	long long responseLenght = responseBody.getSize();

	response += "HTTP/1.1 "; // This is always true
	response += toString(status);
	response += " " + getStatusText(status);
		
	response += "Content Lenght: ";
	response += toString(responseLenght);
	response += "\r\n";

	if (this->status == 301 || this->status == 302) {
		if (redirectUrl != "") {
			response += "Location: ";
			response += redirectUrl;
			response += "\r\n";
		} else {
			response += "Location: ";
			response += resource.substr(resource.find_last_of('/') + 1) + "/";
			response += "\r\n";
		}
	}

	if (location.getCgi() == "" || !checkDirectory(resource)) {
		response += "Content-Type: ";
		response += responseBody.getType();
		response += "\r\n";
	} else {
		response += "Content-Type: ";
		response += "text/html";
		response += "\r\n";
	}

	response += "\r\n";
	response += makeString(responseBody.getBody());

	send(fd, response.c_str(), response.size(), 0);
	sent = true;
}