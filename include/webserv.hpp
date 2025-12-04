#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netdb.h>
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
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <list>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <utility>

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

#define DEFAULT_ERROR_PAGE "www/html/default_error_page.html"
#define MAX_HEADER_SIZE 16000

extern bool	sigstop;
extern int	errorCode;
extern std::string errorLine;
extern char **global_envp;

#define EPOLL_EVENT_COUNT 100
#define BUFFER_SIZE 1024

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

std::string 				strTrim(std::string str);
std::string 				rtrim(std::string str);
std::string 				rtrim(std::string str);
std::string 				strTrim(std::string str, char delimiter);
std::string 				rtrim(std::string str, char delimiter);
std::string 				ltrim(std::string str, char delimiter);
std::string					trimLastWord(std::string str, char delimiter);
bool						strIsDigit(std::string const str);
size_t						countWords(std::stringstream& ss);
size_t						countWords(std::string const str);
std::vector<std::string>	strSplit(const std::string& str, const std::string& delimiter);
size_t						cstrlen(const char *str);
size_t						cstrcat(char *dst, const char *src);

std::string	makeString(const std::vector<char> vec);
std::string	makeString(std::vector<char>::iterator start, std::vector<char>::iterator end);

char		*makeCString(const std::vector<char> vec);
char		*makeCString(std::vector<char>::iterator start, std::vector<char>::iterator end);

std::string					toString(int n);
std::string					getTime();

bool	checkDirectory(std::string resource);

#include "File.hpp"

class File;

void	teapotGenerator(File &responseBody);
File	generateAutoIndex(std::string resource, std::string directory);

std::string	getMIME(std::string needle, bool reverse);


#endif