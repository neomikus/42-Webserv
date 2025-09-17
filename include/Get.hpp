#ifndef GET_HPP
#define GET_HPP

#include "Request.hpp"

class Get: public Request {
	private:

		Get();
		bool		checkRedirect(Location &location, int &status);
		bool		checkIndex(Location &location, File &responseBody);
		bool		checkAutoindex(Location &location, File &responseBody);
		void		getBody(int &status, Location &currentLocation, File &responseBody);
		std::string cgi(int &status, Location &location);
	public:
		Get(std::vector<std::string> splitedResponse);
		Get(const Get &model);
		~Get();
	
		void	response(int fd, std::list<int> &clients, Server &server);
};

#endif