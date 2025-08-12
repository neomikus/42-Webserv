#ifndef POST_HPP
#define POST_HPP

#include "Request.hpp"
#include "File.hpp"

class File;

class Post: public Request {
	private:
		//File		body;
		std::vector<char>	body;
		std::string	contentType;
		std::string	boundary;
		Post();
		void		parseBody(std::vector<char> &rawBody);
		std::string	updateResource(int &status);
	public:
		Post(std::vector<std::string> splittedResponse, std::vector<char> &rawBody);
		//Post	&operator=(const Post &model);
		Post(const Post &model);
		~Post();

		void	response(int fd, std::list<int> &clients, Server &server);
};

#endif