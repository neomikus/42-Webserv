#include <sys/epoll.h>
#include <sys/socket.h>
#include <iostream>
#include <vector>
#include <istream>
#include <fstream>
#include <map>

struct error_page {
	std::vector<int>			to_catch;
	int							to_replace;
	std::string					page;
};

enum cgi_options {
	BASH,
	PHP,
	PYTHON,
	GO,
	NONE
};

struct location
{
	std::string					uri;
	std::string					root;
	std::vector<std::string>	index;
	bool						autoindex;
	std::vector<error_page>		error_pages;
	cgi_options					cgi = cgi_options::NONE;
	location					*location;

};


class Server {
private:
	
	std::string					server_name;
	std::vector<std::string>	host;
	std::vector<int>			port;
	std::vector<error_page>		error_pages;
	int							max_body_size;
	bool						autoindex;
	std::string					root;
	std::vector<std::string>	index;
	std::vector<location>		locations;

	void Server::parsePorts(std::string hostPorts);
	void Server::parseErrorPages(std::string hostPorts);

public:

	Server();
	Server(std::ifstream &confFile);
	~Server();
};
