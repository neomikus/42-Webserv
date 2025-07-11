#ifndef POST_HPP
#define POST_HPP

#include "Request.hpp"

class Post: public Request {
	private:

	Post();
	public:
		Post(std::vector<std::string> splitedResponse);
		Post(const Post &model);
		~Post();
};

#endif