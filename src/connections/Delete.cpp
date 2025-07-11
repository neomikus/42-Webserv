#include "webserv.hpp"
#include "Delete.hpp"

Delete::Delete(){}

Delete::Delete(std::vector<std::string> splitedResponse): Request(splitedResponse){

}

Delete::Delete(const Delete &model): Request(model) {

}

Delete::~Delete(){}