#ifndef POST_HPP
#define POST_HPP

#include "Request.hpp"
#include "File.hpp"

class File;

class Post: public Request {
	private:
		File		body;
		std::string	contentType;
		Post();
		void	parseBody(std::string &rawBody);
		std::string	updateResource();
	public:
		Post(std::vector<std::string> splitedResponse, std::string &rawBody);
		//Post	&operator=(const Post &model);
		Post(const Post &model);
		~Post();

		void	response(int fd, std::list<int> &clients, Server &server);
};

#endif