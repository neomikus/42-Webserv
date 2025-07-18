#include "webserv.hpp"

std::string	getStatusText(int status) {
	switch (status)
	{
		case 200:
			return ("OK\r\n");
		case 404:
			return ("Not Found\r\n");
		case 405:
			return ("Method Not Allowed\r\n");
		case 418:
			return ("I'm a teapot\r\n");
		case 501:
			return ("Not Implemented\r\n");
		default:
			break;
	}
	return ("\r\n");
}
