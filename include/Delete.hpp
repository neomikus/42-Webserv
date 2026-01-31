#ifndef DELETE_HPP
#define DELETE_HPP

#include "Request.hpp"

class Delete: public Request {
	private:
		void	parseHeader(std::vector<Server> &servers);
		void    getBody(File &responseBody);
		void	deleteResource();

	public:
		Delete();
		//Delete(std::vector<std::string> splitedResponse);
		Delete(const Delete &model);
		~Delete();
		void	response(int fd);
};

#endif