#ifndef GENERIC_HPP
#define GENERIC_HPP

#include "Request.hpp"

class Generic: public Request {
	private:

	Generic();
	public:
		Generic(std::vector<std::string> splitedResponse);
		Generic(const Generic &model);
		~Generic();
};

#endif