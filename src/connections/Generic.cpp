#include "webserv.hpp"
#include "Generic.hpp"

Generic::Generic(){}

Generic::Generic(std::vector<std::string> splitedResponse): Request(splitedResponse){

}

Generic::Generic(const Generic &model): Request(model) {

}

Generic::~Generic(){}