#ifndef WEBSERV_HPP
#define WEBSERV_HPP

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

typedef std::pair<std::string, int> hostport;

std::string					parseServerName(std::string value);
hostport					parseHostPort(std::string value);
error_page					parseErrorPage(std::string value);
int							parseBodySize(std::string value);
bool						parseAutoindex(std::string value);
std::string					parseRoot(std::string value);
std::vector<std::string>	parseIndex(std::string value);
cgi_options					parseCgi(std::string value);

std::string trim(std::string str);

#endif