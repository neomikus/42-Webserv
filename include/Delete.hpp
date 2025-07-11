#ifndef DELETE_HPP
#define DELETE_HPP

#include "Request.hpp"

class Delete: public Request {
	private:

	Delete();
	public:
		Delete(std::vector<std::string> splitedResponse);
		Delete(const Delete &model);
		~Delete();
};

#endif