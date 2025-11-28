#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "webserv.hpp"
#include "Server.hpp"

// MAYBE we can make it abstract and make a class for method?
class Request
{
	private:
		std::vector<Server> 				candidates;

	protected:
		std::vector<char>					rawHeader;
		std::vector<char>					rawBody;
		bool								error; // necesary : (firstline)method resource protocol
		std::string							method;
		std::string							query;
		std::string							resource;
		std::string							protocol;

		bool								headerRead;

		long								contentLength;
		hostport							hostPort;
		std::string							userAgent; // I don't know if this is useful to us or not, maybe for cookies?
		std::vector<std::string>			accept; // May be renamed acceptFormat?
		// Accept-Language is horrible
		std::string							transferEncoding;
		bool								keepAlive; // Connection: keep-alive = true, Connection: close = false
		std::string							referer;

		Location							location;

		void						parseMethodResourceProtocol(std::string line);
		bool						checkPermissions();
		int							getStatus(Location &currentLocation);
		virtual void				getBody(int &status, Location &currentLocation, File &responseBody);
		virtual	void				writeContent(File &fileBody) {(void)fileBody;};
		std::string					checkErrorPages(std::vector<error_page> error_pages, int &status);
		void						getErrorPages(std::string &error_page, File &responseBody);
		bool						readHeader(int fd);
		void						parseHeader();
	
	public:
		
		Request();
		Request(std::vector<std::string> splitedRaw);
		Request(const Request &model);
		Request	&operator=(const Request &model);
		virtual ~Request();
		Server	&selectServer(std::vector<Server> &servers);
		virtual void	response(int fd, Server &server); // May return int for response code or for error check?

		std::string			&getMethod();
		std::string			&getProtocol();
		std::string			&getResource();
		std::string			&getQuery();
		std::vector<char>	&getRawRequest();
		Location			&getLocation();

};

std::string	getStatusText(int status);
std::string cgi(int &status, std::string resource, std::string command);


#endif