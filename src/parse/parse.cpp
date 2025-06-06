#include "Webserv.hpp"

std::string	parseServerName(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;

	if (value.empty() || value.find('\t') != value.npos || value.find(' ') != value.npos)
	{
		std::cout << "ERROR" << std::endl;
		exit(0);
	}
	return(value);
}

hostport parseHostPort(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;
	if (value.empty())
	{
		std::cout << "ERROR" << std::endl;
		exit(0);
	}

	std::string			host = "";
	int					port = -1;
	std::stringstream	check;

	if (value.find(':') != value.npos)
	{
		host = value.substr(0, value.find(':'));
		port = atoi(value.substr(host.size() + 1, value.size()).c_str());
		check << port;
		if (check.str().size() == value.size() - host.size() - 1)
			return std::make_pair(host, port);
	{
		std::cout << "ERROR" << std::endl;
		exit(0);
	}
	}

	for (std::string::iterator it = value.begin(); it != value.end(); it++)
		if (!isdigit(*it))
			return std::make_pair(value, -1);
	return std::make_pair(host, atoi(value.c_str()));

}

error_page	parseErrorPage(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;

	if (value.empty() || value.find('/') == value.npos)
	{
		std::cout << "ERROR" << std::endl;
		exit(0);
	}
	
	error_page	error_page;
	error_page.page = "";
	error_page.to_replace = 0;

	std::string	strPage;
	std::string	redirect;
	std::string	error_codes;

	strPage = strTrim(value.substr(value.find('/') + 1));

	value = strTrim(value.substr(0, value.find('/')));

	if (strPage.find(' ') != strPage.npos)
	{
		std::cout << "ERROR" << std::endl;
		exit(0);
	}

	if (value.find('=') != value.npos)
	{
		error_codes = strTrim(value.substr(0, value.find('=')));
		redirect = strTrim(value.substr(value.find('=') + 1));	}
	else
	{
		error_codes = value;
		error_page.to_replace = -1;
	}

	if (error_codes.empty() || (!redirect.empty() && !strIsDigit(redirect)))
	{
		std::cout << "ERROR" << std::endl;
		exit(0);
	}

	if (error_codes.find(' ') != error_codes.npos)
	{
		std::stringstream	split(error_codes);
		while (split)
		{
			std::string			error_c;
			split >> error_c;
			if(error_c.empty())
				break ;
			if (!strIsDigit(error_c))
				exit (0);
			error_page.to_catch.push_back(atoi(error_c.c_str()));
		}
		
	}
	else if (strIsDigit(error_codes))
		error_page.to_catch.push_back(atoi(error_codes.c_str()));
	else
		exit (0);

	error_page.page = strPage;
	if (error_page.to_replace != -1)
		error_page.to_replace = atoi(redirect.c_str());

	return(error_page);
}

cgi_options	parseCgi(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;
	std::string	cgiStr[5] = {"BASH", "PHP", "PYTHON", "GO", "NONE"};

	for (size_t i = 0; i < 5; i++)
	{
		if (value == cgiStr[i])
			return (static_cast<cgi_options>(i));
	}
	exit (0);
}

long long parseBodySize(const std::string value) {
	//std::cout << "[" << value << "]" << std::endl;
	size_t i = 0;

	std::map<std::string, int> unitMap;
	unitMap["b"] = 0;
	unitMap["k"] = 1;
	unitMap["kb"] = 1;
	unitMap["m"] = 2;
	unitMap["mb"] = 2;
	unitMap["g"] = 3;
	unitMap["gb"] = 3;

	long long multipliers[] = {
		BYTE, KB, MB, GB
	};

	while (i < value.size() && std::isdigit(value[i]))
		++i;

	std::string unit = value.substr(i);

	long long size = atoll(value.substr(0, i).c_str());

	if (i == 0)
		exit (0);
	
	for (i = 0; i < unit.size(); ++i) {
		unit[i] = std::tolower(unit[i]);
	}

	if (unitMap.find(unit) == unitMap.end())
		exit (0);


	return (size * multipliers[unitMap.find(unit)->second]);
}

bool	parseAutoindex(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;
	if (value == "on")
		return (true);
	if (value == "off")
		return(false);
	exit (0);
}

std::string	parseRoot(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;
	if (value.find(' ') != value.npos || value.find('\t') != value.npos)
		exit (0);
	return(value);
}

std::vector<std::string>	parseIndex(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;
	std::vector<std::string> indexVector;

	std::stringstream	split(value);
	while (split)
	{
		std::string			index;
		split >> index;
		if(index.empty())
			break ;
		indexVector.push_back(index);
	}

	return(indexVector);
}

