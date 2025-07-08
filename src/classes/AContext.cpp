#include "AContext.hpp"

Context::Context(){
	allowed_methods a_methods = {false, false, false};
	this->methods = a_methods;
}

Context::~Context(){}

void	Context::parseErrorPage(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;

	if (value.empty() || value.find('/') == value.npos)
	{
		std::cout << "ERROR ERROR PAGE1" << std::endl;
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
		std::cout << "ERROR ERROR PAGE2" << std::endl;
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
		std::cout << "ERROR ERROR PAGE3" << std::endl;
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

	error_pages.push_back(error_page);
}

void	Context::parseAutoindex(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;
	if (value == "on")
		autoindex = true;
	else if (value == "off")
		autoindex = false;
	else
		exit(0);
}

void	Context::parseRoot(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;
	if (value.find(' ') != value.npos || value.find('\t') != value.npos)
		exit (0);
	root = value;
}

void	Context::parseIndex(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;
	std::vector<std::string> indexVector;

	std::stringstream	split(value);
	while (split)
	{
		std::string			tmp_index;
		split >> tmp_index;
		if(tmp_index.empty())
			break ;
		indexVector.push_back(tmp_index);
	}
	this->index = indexVector;
}

void Context::parseAlowedMethods(std::string value){
	//std::cout << "[" << value << "]" << std::endl;
	std::stringstream	split(value);
	while (split)
	{
		std::string			word;
		split >> word;

		if (word.empty())
			break;

		if (word == "GET")
			methods._get = true;
		else if (word == "POST")
			methods._post = true;
		else if (word == "DELETE")
			methods._delete = true;
		else
			exit(0);
	}
}