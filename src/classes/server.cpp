#include "Server.hpp"


Server::Server() {}

Server::~Server() { }

Server::Server(std::ifstream &confFile)
{	
	std::string key_words[9] = {
	"server_name", "listen", "error_page", "client_max_body_size", "location", "autoindex", "root", "index", "error" };
	max_body_size = MB;
	for (std::string buffer; !confFile.eof(); std::getline(confFile, buffer))
	{
		buffer = strTrim(buffer);
		if (buffer.empty() || buffer == ";" || buffer[0] == '#')
			continue;
		if (buffer == "}")
			return ;
		if (buffer.find(';') == buffer.npos && buffer.find("location") == buffer.npos)
		{
			std::cout << "ERROR" << std::endl;
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
				locations.push_back(Location(value, confFile, 1));
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
			strConf << "\n  - [" << (!it->first.empty() ? it->first + ":" : "") << it->second << "]";
		strConf << "\n";
	
	}

	if (!hostports.empty())
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
			strConf << *it;
	}
	return(strConf.str());
}


std::ostream &operator<<(std::ostream &stream, const Server server) {
	stream << server.displayConf();
	return (stream); 
}