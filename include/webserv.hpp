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
	std::vector<int>		to_catch;
	int						to_replace;
	std::string				page;
};

struct allowed_methods {
	bool					_get;
	bool					_post;
	bool					_delete;
};

struct hostport {
	std::string				host;
	int						port;
	bool					default_server;
	int						socket;
};

enum cgi_options {
	BASH,
	PHP,
	PYTHON,
	GO,
	NONE
};

std::string 				strTrim(std::string str);
bool						strIsDigit(std::string const str);
size_t						countWords(std::stringstream& ss);
size_t						countWords(std::string const str);
std::vector<std::string>	strSplit(const std::string& str, const std::string& delimiter);

#endif