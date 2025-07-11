#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "webserv.hpp"
#include "Server.hpp"

// MAYBE we can make it abstract and make a class for method?
class Request
{
	protected:
		bool						error; // necesary : (firstline)method resource protocol
		std::string					method;
		std::string					resource;
		std::string					protocol; // This is maybe not needed
							  // Can be checked in constructor if it's HTTP/1.1 or not

		hostport					hostPort;
		std::string userAgent; // I don't know if this is useful to us or not, maybe for cookies?
		std::vector<std::string>	accept; // May be renamed acceptFormat?
		// Accept-Language is horrible
		std::vector<std::string>	acceptEncoding;
		bool						keepAlive; // Connection: keep-alive = true, Connection: close = false
		std::string					referer;
		// Sec fetch: Do later

		void						parseMethodResourceProtocol(const std::string line);
		int							getStatus(const Server &server);
		std::string					getBody(int  &status, const Server &server);
		std::string					getErrorPages(std::string &error_page);
	public:
		
		Request();
		Request(const Request &model);
		Request(std::vector<std::string> splitedRaw);
		virtual ~Request();
		Server	&selectServer(std::vector<Server> &servers, int fd);
		void	response(int fd, std::list<int> &clients, const Server &server); // May return int for response code or for error check?
};

#endif