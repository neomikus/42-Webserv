#include "webserv.hpp"

bool	checkDirectory(std::string resource) {
	struct stat resBuffer;

	stat(resource.c_str(), &resBuffer);

	if (!S_ISDIR(resBuffer.st_mode))
		return (false);
	return (true);
}