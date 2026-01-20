#ifndef GET_HPP
#define GET_HPP

#include "Request.hpp"

class Get: public Request {
	private:
		std::string redirectUrl;

		bool	checkRedirect();
		bool	checkIndex(File &responseBody);
		bool	checkAutoindex(File &responseBody);
		void	getBody(File &responseBody);
		bool	checkAcceptedFormats(File &responseBody);	
		void	acceptedFormats(File &responseBody);
		void	parseHeader(std::vector<Server> &servers);
		void	cgi(int epfd);
	public:
		Get();
		//Get(std::vector<std::string> splitedResponse);
		void	cgiResponse(int fd, int epfd);
		Get(const Get &model);
		~Get();
	
		void	response(int fd);
};

#endif