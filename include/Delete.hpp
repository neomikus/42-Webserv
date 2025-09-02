#ifndef DELETE_HPP
#define DELETE_HPP

#include "Request.hpp"

class Delete: public Request {
	private:
		Delete();
		void    getBody(int &status, Location &currentLocation, File &responseBody);
		void	deleteResource(int &status);

	public:
		Delete(std::vector<std::string> splitedResponse);
		Delete(const Delete &model);
		~Delete();
		void	response(int fd, std::list<int> &clients, Server &server);
};

#endif