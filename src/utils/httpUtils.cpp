#include "webserv.hpp"

std::string	getStatusText(int status) {
	switch (status)
	{
		case 200:
			return ("OK\r\n");
		case 201:
			return ("Created \r\n");
		case 204:
			return ("No Content\r\n");
		case 301:
			return ("Moved permanently\r\n");
		case 302:
			return ("Found\r\n");
		case 400:
			return ("Bad Request\r\n");
		case 403:
			return ("Forbidden\r\n");
		case 404:
			return ("Not Found\r\n");
		case 405:
			return ("Method Not Allowed\r\n");
		case 406:
			return ("Not Acceptable\r\n");
		case 408:
			return ("Request Tiemout\r\n");
		case 411:
			return ("Lenght Required\r\n");
		case 413:
			return ("Payload Too Large\r\n");
		case 418:
			return ("I'm a teapot\r\n");
		case 500:
			return ("Internal Server Error\r\n");
		case 501:
			return ("Not Implemented\r\n");
		case 502:
			return ("Bad Gateway\r\n");
		case 505:
			return ("HTTP Version Not Supported\r\n");
		default:
			break;
	}
	return ("\r\n");
}
