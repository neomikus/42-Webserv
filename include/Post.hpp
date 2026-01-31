#ifndef POST_HPP
#define POST_HPP

#include "Request.hpp"
#include "File.hpp"

class File;

class Post: public Request {
	private:
		typedef std::vector<File>	files;
		bool						body_parsed;
		int							inpipefd[2];

		files				filesVector;
		std::string			contentType;
		std::string			boundary;
		std::string			newResourceName;
		void				parseChunkedData();
		void				parseMultipartData();
		void				parseFormData();
		void				parsePlainData();
		void				parseBody();
		void				updateResource();
		void				writeContent(File &fileBody);
		void				getBody(File &responseBody);
		void				parseHeader(std::vector<Server> &servers);

		void				cgi(int epfd);
		
	public:
		Post();
		Post		&operator=(const Post &model);
		Post(const Post &model);
		~Post();

		std::vector<File>	&getFilesVector();
		void	response(int fd);
		void	cgiResponse(int fd, int epfd);
};

#endif