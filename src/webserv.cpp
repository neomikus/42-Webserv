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

void Server::parsePorts(std::string hostPorts)
{

}
void Server::parseErrorPages(std::string hostPorts)
{

}

Server::Server(std::ifstream &confFile)
{
	std::string key_words[8] = 
	{"server_name", "listen", "error_page", "client_max_body_size", "location", "autoindex", "root", "index" };
	for (std::string buffer; !confFile.eof(); std::getline(confFile, buffer))
	{
		buffer = trim(buffer);
		std::cout << buffer << std::endl;
		int key;
		for (key = 0; key < 8; key++)
			if (buffer.substr(0, buffer.find_first_of(' ')) == key_words[key])
				break;
		switch (key)
		{
			case 0:
				server_name = buffer.substr(key_words[key].size(), buffer.find_first_of(';'));
				break;
			case 1:
				parsePorts(buffer.substr(key_words[key].size(), buffer.find_first_of(';')));
				break;
			case 2:
				parseErrorPages(buffer.substr(key_words[key].size(), buffer.find_first_of(';')));
				break;
		
			default:
				break;
		}


		
	}
}

Server::~Server() { }
