#include "webserv.hpp"
#include "Post.hpp"

Post::Post(): Request() {
	contentType = "none";
}

std::vector<char>::iterator myHaystack(std::vector<char> &haystack, std::vector<char>::iterator begin, std::string needle)
{
	std::vector<char>::iterator itH = begin;
	std::string::iterator itN = needle.begin();
	for (; itH != haystack.end(); ++itH)
	{
		if(*itH == *itN)
		{
			std::vector<char>::iterator retval = itH;
			for (; itN != needle.end() && itH != haystack.end() && *itH == *itN;)
			{
				itH++;
				itN++;
			}
			if (itN == needle.end())
				return retval;
			else
			{
				itN = needle.begin();
				itH = retval + 1;
			}
		}
	}
	return haystack.end();
}
std::string getFilename(std::string filehead)
{
	std::vector<std::string> splited = strSplit(filehead, "\n");
	for (std::vector<std::string>::iterator it = splited.begin(); it != splited.end(); ++it)
	{
		if (it->find("filename=") != it->npos)
			return it->substr((it->find("filename=") + strlen("filename=") + 1), it->size() - (it->find("filename=") + strlen("filename=") + 1) - 2); 
	}
	return "";
}

void	Post::parseMultipartData(std::vector<char> &rawBody) {
	std::vector<char>::iterator lastEnd = rawBody.begin();
	std::vector<char>::iterator start = rawBody.begin();

	for (; start != rawBody.end(); ++start)
	{
		std::vector<char>	body;

		for (; start != rawBody.end(); ++start)
			if (std::distance(start, rawBody.end()) > 4 && *start == '\r' && *(start + 1) == '\n' && *(start + 2) == '\r' && *(start + 3) == '\n')
				break;
		
		std::string fileHead = makeString(lastEnd, start);

		std::string	filename = getFilename(fileHead);

		std::vector<char>::iterator end = myHaystack(rawBody, start, boundary);
		if (end == rawBody.end())
			break;
		if (start == rawBody.end())
			break ;
		start += 4;

		for (; start != end - 4; ++start)
			body.push_back(*start);

		filesVector.push_back(std::make_pair(filename, body));
		lastEnd = end;
		start = end;
	}
}

void	Post::parseFormData(std::string rawBody) {
	std::string	body;
	std::string	filename = "temp.json";

	/* DEFINE FILE NAME HERE */

	body = "{";
	body += "\n";
	std::vector<std::string> formData = strSplit(rawBody, "&");
	for (std::vector<std::string>::iterator it = formData.begin(); it != formData.end(); ++it) {
		body += '\t';
		std::vector<std::string> values = strSplit(*it, "=");
		body += '\"';
		body += values.front();
		body += '\"';
		body += ':';
		body += ' ';
		body += '\"';
		body += values.back();
		body += "\"";
		if (it + 1 != formData.end())
			body += ',';
		body += '\n';
	}
	
	body += "}";
	body += "\n";

	std::vector<char>	bodyVec;
	for (size_t i = 0; body[i]; i++) {
		bodyVec.push_back(body[i]);
	}

	filesVector.push_back(std::make_pair(filename, bodyVec));
	
}

void	Post::parseBody(std::vector<char> &rawBody) {

		std::cout << contentType << std::endl;
	if (contentType == "application/x-www-form-urlencoded") {
		parseFormData(makeString(rawBody));
	}
	else if (contentType == "multipart/form-data") {
		parseMultipartData(rawBody);
	}
	else {
		; // Treat the rest as text formats (JSON, etc) and if it fails it fails
	}

	resource = resource.substr(1);
}

Post::Post(std::vector<std::string> splitedRaw, std::vector<char> &rawBody)/*: Request(splitedRaw)*/ {
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
		if (_temp == "Content-Type:") {
			tokens >> contentType;
			contentType = rtrim(contentType, ';');
			tokens >> _temp;
			if (_temp.substr(0, cstrlen("boundary=")) == "boundary=") {
				boundary = _temp.substr(cstrlen("boundary="));
			}
		}
	}
	parseBody(rawBody);
}

Post::Post(const Post &model): Request(model) {
	
}

void	Post::writeContent(File &fileBody) {
	if (!newResourceName.empty())
		fileBody.open(newResourceName);
}

bool checkStat(std::string resource, std::string &filename, int &status) {
	struct stat dirBuffer;

	stat(resource.c_str(), &dirBuffer);

	if (S_ISDIR(dirBuffer.st_mode)) {
		if (access(resource.c_str(), F_OK)) {
			status = 404;
			return (false);
		}
		if (filename.empty()) { // Generic file name
			/* Cosas */;
		} 
		else {
			if (resource[resource.size()] != '/')
				resource += '/';
			filename = resource + filename;
		}
	}
	else {
		if (filename.empty()) { // Generic file name
			filename = resource;
		}
		else {
			resource = trimLastWord(resource, '/') + '/';
			if (access(resource.c_str(), F_OK)) {
				return (false);
			}
			filename = resource + filename;
		}
	}

	struct stat resBuffer;

	stat(filename.c_str(), &resBuffer);

	if (S_ISDIR(resBuffer.st_mode)) {
		status = 404;
		return (false);
	}
	if (!(dirBuffer.st_mode & S_IWUSR)) {
		status = 403;
	}
	return (true);
}

void	Post::updateResource(int &status) {
	if (status != 201 && status != 204)
		return;

	if (contentType == "multipart/form-data")
		status = 204;

	for (files::iterator it = filesVector.begin(); it != filesVector.end(); ++it) {
		if (!checkStat(resource, it->first, status)) {
			std::cerr << "File: " << it->first << " can't be written" << std::endl;
			break;
		}

		std::filebuf	fb;
		fb.open(it->first.c_str(), std::ios::binary | std::ios::out);
		std::ostream	newResource(&fb);

		for (std::vector<char>::iterator fileIt = it->second.begin(); fileIt != it->second.end(); ++fileIt) {
			newResource.put(*fileIt);
		}
	}
}

void	Post::response(int fd, std::list<int> &clients, Server &server) {
	Location 	location = selectContext(server.getVLocation(), "");
	int			status = getStatus(location);
	updateResource(status);
	File		responseBody;

	getBody(status, location, responseBody);

	std::cout << "Status: " << status << std::endl;
	std::cout << responseBody.getSize() << std::endl;

	long long contentLenght = responseBody.getSize();

	std::string response;

	response += "HTTP/1.1 "; // This is always true
	response += toString(status);
	response += " " + getStatusText(status);
	// I don't know how much we need to add to the response?
	response += "Content Lenght: ";
	response += toString(contentLenght);
	response += "\r\n";
	
	if (status == 201) {	
		response += "Location: ";
		response += newResourceName;
		response += "\r\n";
	}

	
	response += "\r\n";
	
	// If the created resource is small, send it after creation (code 201)
	// If the created resource is big, send metadata (or nothing, fuck it at this point) (Code 201)
	// If !body, then code 204
	
	for (std::string line; std::getline(responseBody.getStream(), line);) {
		response += line;
	}
	
	std::cout << response << std::endl;
	send(fd, response.c_str(), response.length(), 0);
	close(fd);
	clients.erase(std::find(clients.begin(), clients.end(), fd));
}

Post::~Post(){}