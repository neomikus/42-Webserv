#ifndef POST_HPP
#define POST_HPP

#include "Request.hpp"
#include "File.hpp"

class File;

class Post: public Request {
	private:
		typedef std::vector<File> files;

		files				filesVector;
		std::string			contentType;
		std::string			boundary;
		std::string			newResourceName;
		Post();
		void		parseBody(std::vector<char> &rawBody);
		void		parseMultipartData(std::vector<char> &rawBody);
		void		parseFormData(std::string rawBody);
		void		parsePlainData(std::vector<char> rawBody);
		void		updateResource(int &status);
		void		writeContent(File &fileBody);
	public:
		Post(std::vector<std::string> splittedResponse, std::vector<char> &rawBody);
		Post	&operator=(const Post &model);
		Post(const Post &model);
		~Post();

		void	response(int fd, std::list<int> &clients, Server &server);
};

#endif