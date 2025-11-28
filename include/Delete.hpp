#ifndef DELETE_HPP
#define DELETE_HPP

#include "Request.hpp"

class Delete: public Request {
	private:
		void    getBody(int &status, Location &currentLocation, File &responseBody);
		void	deleteResource(int &status);

	public:
		Delete();
		Delete(std::vector<std::string> splitedResponse);
		Delete(const Delete &model);
		~Delete();
		void	response(int fd, Server &server);
};

#endif