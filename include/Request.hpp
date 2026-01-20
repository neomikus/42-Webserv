#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "webserv.hpp"
#include "Server.hpp"

// MAYBE we can make it abstract and make a class for method?
class Request
{
	private:
		std::vector<Server> 				candidates;
		long long							bodyRead;

	protected:
		bool								readError;
		bool								error; // necesary : (firstline)method resource protocol
		bool								sent;

		std::vector<char>					rawHeader;
		std::vector<char>					rawBody;
		std::string							method;
		std::string							query;
		std::string							resource;
		std::string							protocol;

		bool								headerRead;

		long								contentLength;
		hostport							hostPort;
		std::string							userAgent; // I don't know if this is useful to us or not, maybe for cookies?
		std::vector<std::string>			accept; // May be renamed acceptFormat?
		std::string							transferEncoding;
		bool								keepAlive; // Connection: keep-alive = true, Connection: close = false
		std::string							referer;

		Location							location;
		int									status;

		void						parseMethodResourceProtocol(std::string line);
		bool						checkPermissions();
		int							getStatus();
		virtual void				getBody(File &responseBody);
		virtual	void				writeContent(File &fileBody) {(void)fileBody;};
		std::string					checkErrorPages(std::vector<error_page> error_pages);
		void						getErrorPages(std::string &error_page, File &responseBody);
		virtual void				parseHeader(std::vector<Server> &servers);
	
	public:
		Request();
		//Request(std::vector<std::string> splitedRaw);
		Request(const Request &model);
		Request	&operator=(const Request &model);
		virtual ~Request();
		Server	&selectServer(std::vector<Server> &servers);
		virtual void	response(int fd); // May return int for response code or for error check?
		virtual void	cgiResponse(int fd, int epfd);

		bool				readHeader(int fd, std::vector<Server> &servers);
		bool				readBody(int fd);
		
		bool				getReadError();
		void				setReadError(bool value);
		bool				getSent();

		void				setStatus(int newStatus);

		std::string			&getMethod();
		std::string			&getProtocol();
		std::string			&getResource();
		std::string			&getQuery();
		std::vector<char>	&getRawHeader();
		Location			&getLocation();

};

std::string	getStatusText(int status);

Location	selectContext(Location &location, std::string fatherUri, std::string &resource);

#endif