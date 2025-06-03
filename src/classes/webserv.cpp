#include "Webserv.hpp"

std::string trim(std::string str)
{
	for (std::string::iterator it = str.begin(); it != str.end(); it++)
	{
		if (*it != ' ' && *it != '\t')
		{
			str.erase(str.begin(), it);
			return str;
		}
	}
	return str;
}


std::string	parseServerName(std::string value) {
	std::cout << value << std::endl;
	return("NO NAME");
}

hostport parseHostPort(std::string value) {
	std::cout << value << std::endl;
	return std::make_pair("default", 8080);
}

error_page	parseErrorPage(std::string value) {
	std::cout << value << std::endl;
	error_page example;

	example.to_catch.push_back(404);
	example.to_catch.push_back(403);
	example.to_catch.push_back(402);
	example.to_replace = 200;
	example.page = "error.html";


	return(example);
}

cgi_options	parseCgi(std::string value) {
	std::cout << value << std::endl;
	return(PHP);
}

int	parseBodySize(std::string value) {
	std::cout << value << std::endl;
	return (32);
}

bool	parseAutoindex(std::string value) {
	std::cout << value << std::endl;
	return(false);
}

std::string	parseRoot(std::string value) {
	std::cout << value << std::endl;
	return("Default");
}

std::vector<std::string>	parseIndex(std::string value) {
	std::cout << value << std::endl;
	std::vector<std::string> defaultIndex;

	defaultIndex.push_back("index1");
	defaultIndex.push_back("index2");
	defaultIndex.push_back("index3");

	return(defaultIndex);

}

