#include "Server.hpp"


Server::Server() {}

Server::Server(const Server& model) {

	server_name = model.server_name;
	hostports = model.hostports;
	error_pages = model.error_pages;
	max_body_size = model.max_body_size;
	autoindex = model.autoindex;
	root = model.root;
	index = model.index;
    for (size_t i = 0; i < model.locations.size(); i++) {
        locations.push_back(Location(model.locations[i]));
    }
	methods = model.methods;
}

Server &Server::operator=(const Server &model) {

	server_name = model.server_name;
	hostports = model.hostports;
	error_pages = model.error_pages;
	max_body_size = model.max_body_size;
	autoindex = model.autoindex;
	root = model.root;
	index = model.index;
    for (size_t i = 0; i < model.locations.size(); i++) {
        locations.push_back(Location(model.locations[i]));
    }
	methods = model.methods;

	return(*this);
}

Server::~Server() {

}

void Server::parseBodySize(const std::string value) {
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
	
	this->max_body_size = (size * multipliers[unitMap.find(unit)->second]);
}


void Server::parseHostPort(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;
	if (value.empty())
	{
		std::cout << "ERROR HOST PORT1" << std::endl;
		exit(0);
	}

	hostport hostport;

	hostport.host = "0.0.0.0";
	hostport.port = 80;
	hostport.default_server = false;

	std::stringstream	split(value);
	size_t nOfWords = countWords(split);

	if (nOfWords > 2)
		exit(0);

	std::string firstWord;
	std::string secondWord;
	split >> firstWord;
	split >> secondWord;


	if (firstWord.find(':') != value.npos)
	{
		hostport.host = firstWord.substr(0, firstWord.find(':'));
		hostport.port = atoi(firstWord.substr(hostport.host.size() + 1, firstWord.size()).c_str());
	
		std::stringstream	check;
		check << hostport.port;
	
		if (check.str().size() != firstWord.size() - hostport.host.size() - 1)
		{
			std::cout << "ERROR HOST PORT2" << std::endl;
			exit(0);
		}
	}
	else if (strIsDigit(firstWord))
		hostport.port = atoi(firstWord.c_str());
	else if (firstWord == "default_server")
		exit(0);
	else
		hostport.host = firstWord;
	if (secondWord == "default_server")
		hostport.default_server = true;
	hostports.push_back(hostport);
}

void	Server::parseServerName(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;

	if (value.empty() || value.find('\t') != value.npos || value.find(' ') != value.npos)
	{
		std::cout << "ERROR SERVER NAME" << std::endl;
		exit(0);
	}
	server_name = value;
}

Server::Server(std::ifstream &confFile)
{	
	std::string key_words[10] = {
	"server_name", "listen", "error_page", "client_max_body_size", "location", "autoindex", "root", "index", "allow_methods", "error" };
	max_body_size = MB;
	for (std::string buffer; std::getline(confFile, buffer);)
	{
		buffer = strTrim(buffer);
		if (buffer.empty() || buffer == ";" || buffer[0] == '#')
			continue;
		if (buffer == "}")
			return ;
		if (buffer.find(';') == buffer.npos && buffer.find("location") == buffer.npos)
		{
			std::cout << "ERROR4" << std::endl;
			exit(0);
		}
		int key;
		for (key = 0; key < 8; key++)
			if (buffer.substr(0, buffer.find_first_of(' ')) == key_words[key])
				break;
		std::string value = buffer.substr(key_words[key].size(), buffer.find_first_of(';') - key_words[key].size());
		value = strTrim(value);
		switch (key)
		{
			case 0:
				parseServerName(value);
				break;
			case 1:
				parseHostPort(value);
				break;
			case 2:
				parseErrorPage(value);
				break;
			case 3:
				parseBodySize(value);
				break;
			case 4:
				locations.push_back(Location(value, confFile, 1));
				break;
			case 5:
				parseAutoindex(value);
				break;
			case 6:
				parseRoot(value);
				break;
			case 7:
				parseIndex(value);
				break;
			case 8:
				parseAlowedMethods(value);
				break;
			default:
				break;
		}		
	}
}

std::string	Server::displayConf() const {
	std::stringstream strConf;
	strConf << HBLU;
	if (!server_name.empty())
		strConf << "| SERVER NAME\t: " << server_name <<  + "\n";
	if (max_body_size != MB)
		strConf << "| MAX BODY SIZE\t: " << max_body_size <<  + "\n";

	if (autoindex)
	strConf << "| AUTOINDEX\t: ON" << "\n";
	
	if (!root.empty())
		strConf << "| ROOT\t\t: " << root << "\n";

	if (!index.empty())
	{
		strConf << "| INDEX\t\t:";
		for (std::vector<std::string>::const_iterator it = index.begin(); it != index.end(); it++)
			strConf << "\n  - [" << *it << "]";
		strConf << "\n";
	}

	if (!hostports.empty())
	{
		strConf << "| HOST PORTS\t:";
		for (std::vector<hostport>::const_iterator it = hostports.begin(); it != hostports.end(); it++)	
			strConf << "\n  - [" << (!it->host.empty() ? it->host + ":" : "") << it->port << "]" << (it->default_server ? " defaul_server" : "");
		strConf << "\n";
	}

	if (methods._delete || methods._get || methods._post)
	{
		strConf << "| METHODS\t:";
		strConf << (methods._get ? " GET" : "");
		strConf << (methods._post ? " POST" : "");
		strConf << (methods._delete ? " DELETE" : "");
		strConf << "\n";
	}

	if (!error_pages.empty())
	{
		strConf << "| ERROR PAGES\t:";
		for (std::vector<error_page>::const_iterator it = error_pages.begin(); it != error_pages.end(); it++)
		{
			strConf << "\n  - [";
			for (std::vector<int>::const_iterator it_catch = it->to_catch.begin(); it_catch != it->to_catch.end(); it_catch++)
				strConf << *it_catch << " ";
			if (it->to_replace != -1)
				strConf << "= " << it->to_replace;
			strConf << " \\ " << it->page << "]";
		}
		strConf << "\n";
	}

	if (!locations.empty())
	{
		strConf << "| LOCATIONS\t:\n";
		for (std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); it++)
			strConf << it->displayConf();
	}
	return(strConf.str());
}


std::ostream &operator<<(std::ostream &stream, const Server server) {
	stream << server.displayConf();
	return (stream); 
}