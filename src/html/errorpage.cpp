#include "webserv.hpp"
#include "File.hpp"
#include "Request.hpp"

void	errorPageGenerator(File &responseBody, int &status) {

	std::stringstream contents;

	responseBody.write("<!DOCTYPE html>\n");
	responseBody.write(std::string("<h1 id='error'>" + toString(status) + " " + getStatusText(status) + "</h1>\n").c_str());
	switch (status) {
		case 400:
			responseBody.write(std::string("<h1> The request header was either corrupted or had errors </h1>\n").c_str());
			break;
		case 403:
			responseBody.write(std::string("<h1> The requested URL <span id='URI'></span> was not accessible </h1>\n").c_str());
			break;
		case 404:
			responseBody.write(std::string("<h1> The requested URL <span id='URI'></span> was not found in this server </h1>\n").c_str());
			break;
		case 405:
			responseBody.write(std::string("<h1> The requested URL <span id='URI'></span> does not accept the requested method </h1>\n").c_str());
			break;
		case 406:
			responseBody.write(std::string("<h1> The resource sent to server was not acceptable </h1>\n").c_str());
			break;
		case 408:
			responseBody.write(std::string("<h1> The request has timed out </h1>\n").c_str());
			break;
		case 411:
			responseBody.write(std::string("<h1> The request has sent a body but not a Content-Lenght header </h1>\n").c_str());
			break;
		case 413:
			responseBody.write(std::string("<h1> The resource sent was too big for the requested URL </h1>\n").c_str());
			break;
		case 418:
			responseBody.write(std::string("<h1> I'M A TEAPOT!!!! </h1>\n").c_str());
			break;
		case 500:
			responseBody.write(std::string("<h1> Something went bad on our side </h1>\n").c_str());
			break;
		case 502:
			responseBody.write(std::string("<h1> The requested URL was served through CGI, and it seems to have failed </h1>\n").c_str());
			break;
		case 504:
			responseBody.write(std::string("<h1> The requested URL was served through CGI, and it didn't finish executing on time </h1>\n").c_str());
			break;
		case 505:
			responseBody.write(std::string("<h1> The request protocol was not HTTP/1.1 or HTTP/1.0 </h1>\n").c_str());
			break;
		default:
			responseBody.write(std::string("<h1> Unknown error </h1>\n").c_str());
	}

	responseBody.write("<hr>\n");
	responseBody.write("<div>\n\t<p id ='serverinfo'></p>\n</div>\n");
	responseBody.write("<script>\ndocument.getElementById('serverinfo').innerHTML = '42-Webserv/0.0.1 at ' + window.location.hostname + ' on port ' + window.location.port;\ndocument.getElementById('URI').innerHTML = window.location.pathname;\n</script>\n");

	responseBody.setType("text/html");
}