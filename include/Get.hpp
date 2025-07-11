#ifndef GET_HPP
#define GET_HPP

#include "Request.hpp"

class Get: public Request {
	private:

	Get();
	public:
		Get(std::vector<std::string> splitedResponse);
		Get(const Get &model);
		~Get();
};

#endif