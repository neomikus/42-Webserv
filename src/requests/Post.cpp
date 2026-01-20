#include "webserv.hpp"
#include "Post.hpp"

Post::Post(): Request() {
	contentType = "none";
	body_parsed = false;
}

std::vector<char> makeChunk(std::vector<char>::iterator begin, std::vector<char>::iterator end) {
	std::vector<char> chunk;
	for (; begin != end; ++begin) {
		chunk.push_back(*begin);
	}
	return (chunk);
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

bool	finalChunk(std::vector<char> &chunk) {
	if (chunk.empty())
		return (false);
	if (chunk[0] != '0')
		return (false);
	if (chunk[1] != '\r' || chunk[2] != '\n')
		return (false);
	return (true);
}

void	Post::parseChunkedData() {
	std::string	totalSize;

	std::vector<char>::iterator it = rawBody.begin();
	for (; *it != '\n'; ++it) {
		totalSize += *it;
	}
	
	std::vector<char>::iterator start = it + 1;
	std::vector<char>::iterator end;
	File	currentFile;

	for (std::vector<char> currentChunk; ;) {
		std::string	size;

		for (; *start != '\r' && *start != '\n'; ++start) {
			size += *start;
		}
		
		if (strtol(size.c_str(), NULL, 16) == 0)
			break;
		
		start += 2;
		end = start;
		std::advance(end, strtol(size.c_str(), NULL, 16));

		currentChunk = makeChunk(start, end);

		currentFile.write(currentChunk.begin(), currentChunk.end());
		start = end + 2;
	}
	filesVector.push_back(currentFile);
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

void	Post::parseMultipartData() {
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
		if (start == rawBody.end())
			break;
		start += 4;

		File	newFile(filename, start, end - 4);		

		filesVector.push_back(newFile);
		
		if (end == rawBody.end())
			break;
	
		lastEnd = end;
		start = end;
	}
}

/*
	Need to see if using timestamps as filenames or more RESTful UUIDs
	https://digitalbunker.dev/understanding-how-uuids-are-generated/
	https://en.wikipedia.org/wiki/Universally_unique_identifier
*/
void	Post::parseFormData() {
	std::string	body;

	std::string	filename;
	if (!checkDirectory(resource))
		filename = resource;
	else
		filename = getTime() + ".json";
	std::string toParse = makeString(rawBody);

	body = "{";
	body += "\n";
	std::vector<std::string> formData = strSplit(toParse, "&");
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

	File	newFile;

	newFile.setName(filename);
	newFile.write(body.c_str());
	newFile.setType("application/json");

	filesVector.push_back(newFile);
}

void	Post::parsePlainData() {
	std::string	filename = "temp" + getMIME(contentType, true);
	File	newFile;

	newFile.setName(filename);
	newFile.getBody() = rawBody;
	newFile.getSize() = makeString(rawBody).length();

	filesVector.push_back(newFile);
}

void	Post::parseBody() {

	if (transferEncoding == "chunked") {
		parseChunkedData();
	} else if (contentType == "application/x-www-form-urlencoded") {
		parseFormData();
	} else if (contentType == "multipart/form-data") {
		parseMultipartData();
	} else {
		parsePlainData();
	}
	body_parsed = true;
}

Post::Post(const Post &model): Request(model) {
	this->contentType = model.contentType;
}

Post	&Post::operator=(const Post &model) {
	this->error = model.error;
	this->method = model.method;
	this->resource = model.resource;
	this->protocol = model.protocol;
	this->hostPort = model.hostPort;
	this->userAgent = model.userAgent;
	this->accept = model.accept;
	this->keepAlive = model.keepAlive;
	this->referer = model.referer;
	this->contentType = model.contentType;
	return (*this);
}

void	Post::parseHeader(std::vector<Server> &servers) {
	std::vector<std::string>	header = strSplit(makeString(rawHeader), "\r\n");

	for (std::vector<std::string>::iterator it = header.begin(); it != header.end(); ++it)
	{
		*it = strTrim(*it, '\r');

		if (it->find("Host") != it->npos) {
			hostPort.host = strTrim(it->substr(it->find("Host: "), it->find(':')));
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
		if (it->find("Content-Length") != it->npos) {
			contentLength = atol(strTrim(it->substr(cstrlen("Content-Length:"))).c_str());
		}
		if (it->find("Content-Type") != it->npos) {
			contentType = strTrim(it->substr(cstrlen("Content-Type:"), it->find(";") - cstrlen("Content-Type:")));
			if (it->find("boundary=") != it->npos) {
				std::string temp = strTrim(it->substr(it->find(";") + 1), ' ');
				boundary = temp.substr(cstrlen("boundary="));
			}
		}
		if (it->find("Transfer-Encoding") != it->npos) {
			transferEncoding = strTrim(it->substr(cstrlen("Transfer-Encoding:")));
		}
	}
	headerRead = true;
	location = selectContext(selectServer(servers).getVLocation(), "", resource);
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
			filename = getTime();
		} 
		else {
			if (resource[resource.size() - 1] != '/')
				resource += '/';
			filename = resource + filename;
		}
	} else {
		if (filename.empty()) { // Generic file name
			filename = resource;
			resource = trimLastWord(resource, '/') + "/";
		}
		else {
			resource = trimLastWord(resource, '/') + "/";
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

	if (!(dirBuffer.st_mode & S_IWUSR) || (!access(filename.c_str(), F_OK) && !(resBuffer.st_mode & S_IWUSR))) {
		status = 403;
		return (false);
	}
	return (true);
}

void	Post::updateResource() {
	if (status != 201 && status != 204)
		return;
	
	for (files::iterator it = filesVector.begin(); it != filesVector.end(); ++it) {
		if (!checkStat(resource, it->getName(), status)) {
			std::cerr << "File: " << it->getName() << " can't be written" << std::endl;
			break;
		}

		std::filebuf	fb;

		fb.open(it->getName().c_str(), std::ios::binary | std::ios::out);
		std::ostream	newResource(&fb);

		for (std::vector<char>::iterator fileIt = it->getBody().begin(); fileIt != it->getBody().end(); ++fileIt) {
			newResource.put(*fileIt);
		}

		newResourceName = it->getName();
	}
}

void	Post::getBody(File &responseBody) {
	if (status == 201) {
		writeContent(responseBody);
	} else if (status == 204) {
		; // Nothing to return!!!
	} else {
		std::string page = checkErrorPages(location.getError_pages());
		if (!page.empty())
			getErrorPages(page, responseBody);
		else
			errorPageGenerator(responseBody, status);
	}
}

void	Post::response(int fd) {
	if (location.getCollectionRoute() != "")
		resource = location.getCollectionRoute();
	if (!location.getRoot().empty())
		resource = location.getRoot() + "/" + resource;
	if (!status)
		status = getStatus();
		
	if (!body_parsed)
		parseBody();
	std::string response;

	updateResource();
	File		responseBody;

	getBody(responseBody);

	long long responseLenght = responseBody.getSize();

	response += "HTTP/1.1 "; // This is always true
	response += toString(status);
	response += " " + getStatusText(status);
		
	response += "Content Lenght: ";
	response += toString(responseLenght);
	response += "\r\n";

	if (status == 201) {
		response += "Location: ";
		response += newResourceName;
		response += "\r\n";
	} else if (status == 204) {
		response += "Location: ";
		response += trimLastWord(newResourceName, '/') + "/";
		response += "\r\n";
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

	send(fd, response.c_str(), response.length(), 0);
	sent = true;
}

Post::~Post(){}