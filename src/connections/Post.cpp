#include "webserv.hpp"
#include "Post.hpp"

Post::Post(){}

Post::Post(std::vector<std::string> splitedResponse): Request(splitedResponse){

}

Post::Post(const Post &model): Request(model) {

}

Post::~Post(){}