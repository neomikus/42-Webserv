#include "webserv.hpp"
#include "Post.hpp"

Post::Post(): Request() {
	contentType = "none";
}

void	Post::parseBody(std::string &rawBody) {
	// Program later
	//if (rawBody.size() > server.getMax_body_size())
	//	return ;
	(void)rawBody;
	if (contentType == "application/x-www-form-urlencoded")
		;
	else if (contentType == "application/json")
		;
}

Post::Post(std::vector<std::string> splitedRaw, std::string &rawBody): Request(splitedRaw){
	//*this = Post();
	parseMethodResourceProtocol(splitedRaw[0]);
	if (error)
		return ;
	for (std::vector<std::string>::iterator it = splitedRaw.begin(); it != splitedRaw.end(); it++)
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
		if (_temp == "Content-Type:")
			contentType = it->substr(13);	
	}
	parseBody(rawBody);

	std::cout << "this->host " << hostPort.host << std::endl;
	std::cout << "this->port " << hostPort.port << std::endl;
	std::cout << "this->userAgent " << userAgent << std::endl;
	std::cout << "this->accept" << std::endl;
	for (std::vector<std::string>::iterator it = accept.begin(); it != accept.end(); it++)
		std::cout << '\t' << *it << std::endl;
	std::cout << "this->acceptEncoding" << std::endl;
	for (std::vector<std::string>::iterator it = acceptEncoding.begin(); it != acceptEncoding.end(); it++)
		std::cout << '\t' << *it << std::endl;
	std::cout << "this->keepAlive " << keepAlive << std::endl;
	std::cout << "this->referer " << referer << std::endl;
	std::cout << "this->method " << method << std::endl;
	std::cout << "this->resource " << resource << std::endl;
	std::cout << "this->protocol " << protocol << std::endl;

	std::cout << std::endl << std::endl;
	std::cout << "this->body " << body << std::endl;
}

Post::Post(const Post &model): Request(model) {

}

Post::~Post(){}