#ifndef GET_HPP
#define GET_HPP

#include "Request.hpp"

class Get: public Request {
	private:

		Get();
		void	checkIndex(Location &location);

	public:
		Get(std::vector<std::string> splitedResponse);
		Get(const Get &model);
		~Get();
	
		void	response(int fd, std::list<int> &clients, Server &server);
};

#endif