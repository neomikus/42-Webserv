#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <sys/epoll.h>
#include <sys/socket.h>
#include <string>
#include <sstream>
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
#include <fcntl.h>
#include <cstdlib>
#include <list>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>

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

const long long BYTE = 1LL;
const long long KB = BYTE * 1024;
const long long MB = KB * 1024;
const long long GB = MB * 1024;

#include "Socket.hpp"

struct error_page {
	std::vector<int>			to_catch;
	int							to_replace;
	std::string					page;
};

struct allowed_methods {
	bool						_get;
	bool						_post;
	bool						_delete;
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
long long					parseBodySize(std::string value);
bool						parseAutoindex(std::string value);
std::string					parseRoot(std::string value);
std::vector<std::string>	parseIndex(std::string value);
cgi_options					parseCgi(std::string value);
allowed_methods				parseAlowedMethods(std::string value);

std::string strTrim(std::string str);
bool		strIsDigit(std::string const str);

#endif