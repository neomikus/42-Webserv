#include "webserv.hpp"


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

Server::Server() { }


location Server::parseLocation(std::string value, std::ifstream &confFile) {
	std::cout << value << std::endl;
	location newLocation;

	newLocation.uri = value;
	newLocation.cgi = NONE;
	newLocation.location = NULL;
	std::string key_words[8] = {
	"error_page", "location", "autoindex", "root", "index", "cgi"};
	for (std::string buffer; !confFile.eof(); std::getline(confFile, buffer))
	{
		buffer = trim(buffer);
		if (buffer.empty())
			continue;
		if (buffer == "}")
			return newLocation;
		int key;
		for (key = 0; key < 8; key++)
			if (buffer.substr(0, buffer.find_first_of(' ')) == key_words[key])
				break;
		value = buffer.substr(key_words[key].size(), buffer.find_first_of(';'));
		switch (key)
		{
			case 0:
				newLocation.error_pages.push_back(parseErrorPage(value));
				break;
			case 1:
				*newLocation.location = parseLocation(value, confFile);
				break;
			case 2:
				newLocation.autoindex = parseAutoindex(value);
				break;
			case 3:
				newLocation.root = parseRoot(value);
				break;
			case 4:
				newLocation.index = parseIndex(value);
				break;
			case 5:
				newLocation.cgi = parseCgi(value);
				break;
			default:
				break;
		}		
	}
	return newLocation;
}

std::string	Server::parseServerName(std::string value) {
	std::cout << value << std::endl;
	return("NO NAME");
}

hostport Server::parseHostPort(std::string value) {
	std::cout << value << std::endl;
	return std::make_pair("default", 8080);
}

error_page	Server::parseErrorPage(std::string value) {
	std::cout << value << std::endl;
	error_page example;

	example.to_catch.push_back(404);
	example.to_catch.push_back(403);
	example.to_catch.push_back(402);
	example.to_replace = 200;
	example.page = "error.html";


	return(example);
}

cgi_options	Server::parseCgi(std::string value) {
	std::cout << value << std::endl;
	return(PHP);
}

int	Server::parseBodySize(std::string value) {
	std::cout << value << std::endl;
	return (32);
}

bool	Server::parseAutoindex(std::string value) {
	std::cout << value << std::endl;
	return(false);
}

std::string	Server::parseRoot(std::string value) {
	std::cout << value << std::endl;
	return("Default");
}

std::vector<std::string>	Server::parseIndex(std::string value) {
	std::cout << value << std::endl;
	std::vector<std::string> defaultIndex;

	defaultIndex.push_back("index1");
	defaultIndex.push_back("index2");
	defaultIndex.push_back("index3");

	return(defaultIndex);

}

Server::Server(std::ifstream &confFile)
{	
	std::string key_words[9] = {
	"server_name", "listen", "error_page", "client_max_body_size", "location", "autoindex", "root", "index", "error" };
	for (std::string buffer; !confFile.eof(); std::getline(confFile, buffer))
	{
		buffer = trim(buffer);
		if (buffer.empty())
			continue;
		if (buffer == "}")
			return ;
		int key;
		for (key = 0; key < 8; key++)
			if (buffer.substr(0, buffer.find_first_of(' ')) == key_words[key])
				break;
		std::string value = buffer.substr(key_words[key].size(), buffer.find_first_of(';'));
		switch (key)
		{
			case 0:
				server_name = parseServerName(value);
				break;
			case 1:
				hostports.push_back(parseHostPort(value));
				break;
			case 2:
				error_pages.push_back(parseErrorPage(value));
				break;
			case 3:
				max_body_size = parseBodySize(value);
				break;
			case 4:
				locations.push_back(parseLocation(value, confFile));
				break;
			case 5:
				autoindex = parseAutoindex(value);
				break;
			case 6:
				root = parseRoot(value);
				break;
			case 7:
				index = parseIndex(value);
				break;
			default:
				break;
		}		
	}
}

Server::~Server() { }

std::string	Server::displayConf() const {
	std::stringstream strConf;
	strConf << HBLU << std::setw(30) << std::setfill('-') << "\n";
	strConf << "| SERVER NAME\t: " << server_name <<  + "\n";
	strConf << "| MAX BODY SIZE\t: " << max_body_size <<  + "\n";
	strConf << "| AUTO INDEX\t: " << (autoindex ? "TRUE" : "FALSE") <<  + "\n";
	strConf << "| ROOT\t\t: " << root << "\n";
	strConf << "| INDEX\t\t:";
	if (index.empty())
		strConf << " NONE\n";
	else
	{
		for (std::vector<std::string>::const_iterator it = index.begin(); it != index.end(); it++)
			strConf << "\n- " << " [" << *it << "]";
		strConf << "\n";
	
	}
	strConf << "| HOST PORTS\t:";
	if (hostports.empty())
		strConf << " NONE\n";
	else
	{
		for (std::vector<hostport>::const_iterator it = hostports.begin(); it != hostports.end(); it++)
			strConf << "\n- " << " [" << it->first << ":" << it->second << "]";
		strConf << "\n";
	
	}

	strConf << "| ERROR PAGES\t:";
	if (hostports.empty())
		strConf << " NONE\n";
	else
	{
		for (std::vector<error_page>::const_iterator it = error_pages.begin(); it != error_pages.end(); it++)
		{
			strConf << "\n-  [";
			for (std::vector<int>::const_iterator it_catch = it->to_catch.begin(); it_catch != it->to_catch.end(); it_catch++)
				strConf << *it_catch << " ";
			strConf << "= " << it->to_replace;
			strConf << " \\ " << it->page;
			strConf << "]";
		
		}
		strConf << "\n";
	}

	strConf << "| LOCATION\t:\n";
	if (locations.empty())
		strConf << " NONE\n";
	else
	{
		strConf << HGRE << std::setw(30) << std::setfill('+') << "\n";
		for (std::vector<location>::const_iterator it = locations.begin(); it != locations.end(); it++)
			strConf << *it << std::setw(30) << std::setfill('+') << "\n";
	}
	strConf << HBLU << std::setw(30) << std::setfill('-') << "\n";
	return(strConf.str());
}

std::ostream &operator<<(std::ostream &stream, const Server server) {
	stream << server.displayConf();
	return (stream); 
}


std::ostream &operator<<(std::ostream &stream, const location location) {
	std::string	cgiStr[5] = {"BASH", "PHP", "PYTHON", "GO", "NONE"};

	stream << "-| URI\t\t: " << location.uri << "\n";

	stream << "-| ROOT\t\t: " << location.root << "\n";

	stream << "-| AUTOINDEX\t: " << (location.autoindex ? "TRUE" : "FALSE") << "\n";

	stream << "-| CGI\t\t: " << cgiStr[location.cgi]<< "\n";

	stream << "-| INDEX\t:";
	if (location.index.empty())
		stream << " NONE\n";
	else
	{
		for (std::vector<std::string>::const_iterator it = location.index.begin(); it != location.index.end(); it++)
			stream << "\n- " << " [" << *it << "]";
		stream << "\n";
	}

	stream << "| ERROR PAGES\t:";
	if (location.error_pages.empty())
		stream << " NONE\n";
	else
	{
		for (std::vector<error_page>::const_iterator it = location.error_pages.begin(); it != location.error_pages.end(); it++)
		{
			stream << "\n-  [";
			for (std::vector<int>::const_iterator it_catch = it->to_catch.begin(); it_catch != it->to_catch.end(); it_catch++)
				stream << *it_catch << " ";
			stream << "= " << it->to_replace;
			stream << " \\ " << it->page;
			stream << "]";
		}
		stream << "\n";
	}
	if (location.location)
	{
		stream << HMAG << std::setw(30) << std::setfill('*') << "\n";
		stream << location.location << "\n";
		stream << std::setw(30) << std::setfill('*') << "\n";
	}
	return (stream); 
}