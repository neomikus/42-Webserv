#include "webserv.hpp"
#include "Get.hpp"

Get::Get(){}

Get::Get(std::vector<std::string> splitedResponse): Request(splitedResponse){

}

Get::Get(const Get &model): Request(model) {

}

Get::~Get(){}

void	Get::checkIndex(Location &location) {
	std::vector<std::string> indexes = location.getIndex();
	if (indexes.empty() || !checkDirectory(resource))
		return;
	for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
		if (!access((resource + *it).c_str(), F_OK)) {
			resource += *it;
			break;
		}
	}
}

void	Get::response(int fd, std::list<int> &clients, Server &server) {
	Location 	location = selectContext(server.getVLocation(), "");
	checkIndex(location);
	int	status = getStatus(location);
	File		responseBody;

	getBody(status, location, responseBody);

	long long contentLenght = responseBody.getSize();

	std::string responseTemp;

	responseTemp += "HTTP/1.1 "; // This is always true
	responseTemp += toString(status);
	responseTemp += " " + getStatusText(status);
	// I don't know how much we need to add to the responseTemp?
	responseTemp += "Content Lenght: ";
	responseTemp += toString(contentLenght);
	responseTemp += "\r\n";
	
	responseTemp += "\r\n";
	responseTemp += makeString(responseBody.getBody());

	send(fd, responseTemp.c_str(), responseTemp.size(), 0);
	close(fd);
	clients.erase(std::find(clients.begin(), clients.end(), fd));
}