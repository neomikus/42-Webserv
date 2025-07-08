#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "webserv.hpp"

class Request
{
	private:
		std::string method;
		std::string	resource;
		std::string protocol; // This is maybe not needed
							  // Can be checked in constructor if it's HTTP/1.1 or not

		std::pair<std::string, int>	hostport;
		std::string userAgent; // I don't know if this is useful to us or not, maybe for cookies?
		std::vector<std::string>	accept; // May be renamed acceptFormat?
		// Accept-Language is horrible
		std::vector<std::string>	acceptEncoding;
		bool						keepAlive; // Connection: keep-alive = true, Connection: close = false
		std::string	referer;
		// Sec fetch: Do later
		int	urgency; bool incremental; // Both part of priority, may be saved as double?
		Request();
	public:
		Request(std::string &raw);
		~Request();

		void	response(int fd, std::list<int> &clients); // May return int for response code or for error check?
};

#endif