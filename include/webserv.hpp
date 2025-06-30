#include <sys/epoll.h>
#include <sys/socket.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <istream>
#include <fstream>
#include <utility>
#include <map>
#include <csignal>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <netinet/in.h>

#define HMAG		"\033[95m"
#define HCYA		"\033[96m"
#define HDCY		"\033[36m"
#define HBLU		"\033[94m"
#define HGRE		"\033[92m"
#define HYEL		"\033[93m"
#define HRED		"\033[91m"
#define TBOL		"\033[1m"
#define TULN		"\033[4m"
#define RST			"\033[0m"

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
	cgi_options					cgi;
	location					*location;
};

typedef std::pair<std::string, int> hostport;

class Server {
private:
	
	std::string					server_name;
	//std::vector<std::string>	host;
	//std::vector<int>			port;
	std::vector<hostport>		hostports;
	std::vector<error_page>		error_pages;
	int							max_body_size;
	bool						autoindex;
	std::string					root;
	std::vector<std::string>	index;
	std::vector<location>		locations;

	std::string					parseServerName(std::string value);
	hostport					parseHostPort(std::string value);
	error_page					parseErrorPage(std::string value);
	int							parseBodySize(std::string value);
	location					parseLocation(std::string value, std::ifstream &confFile);
	bool						parseAutoindex(std::string value);
	std::string					parseRoot(std::string value);
	std::vector<std::string>	parseIndex(std::string value);
	cgi_options					parseCgi(std::string value);

public:

	Server();
	Server(std::ifstream &confFile);
	~Server();
	std::string	displayConf() const;
};

std::ostream &operator<<(std::ostream &stream, const Server server);
std::ostream &operator<<(std::ostream &stream, const location location);

