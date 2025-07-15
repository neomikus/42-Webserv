#include "Server2.hpp"


Server::Server() {}

Server::Server(const Server& model) {

	server_name = model.server_name;
	hostports = model.hostports;
	vLocation = Location(model.vLocation);
}

Server &Server::operator=(const Server &model) {

	server_name = model.server_name;
	hostports = model.hostports;
	vLocation = Location(model.vLocation);
	return(*this);
}

Server::~Server() {

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

void	resolveHostPorts(std::vector<hostport> &hostports)
{
	if (hostports.empty())
		return;
	std::vector<hostport>::reverse_iterator it = hostports.rbegin();
	for (; it != hostports.rend(); it++)
		if (it->default_server)
			break;
	if (it != hostports.rend())
	{
		it++;
		for (; it != hostports.rend(); it++)
			it->default_server = false;
	}
}


Server::Server(std::ifstream &confFile)
{	
	std::streampos startPos = confFile.tellg();
	std::string key_words[3] = {
	"server_name", "listen", "error" };

	for (std::string buffer; std::getline(confFile, buffer);)
	{
		buffer = strTrim(buffer);
		if (buffer.empty() || buffer == ";" || buffer[0] == '#')
			continue;
		if (buffer == "}")
		{
			resolveHostPorts(hostports);
			confFile.clear();
			confFile.seekg(startPos);
			vLocation = Location("", confFile, 0);
			return ;
		}
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
			default:
				break;
		}
	}
}

std::ostream &operator<<(std::ostream &stream, Server server) {

	stream << HBLU;
	if (!server.getServer_name().empty())
		stream << "| SERVER NAME\t: " << server.getServer_name() <<  + "\n";
	std::vector<hostport> _hostports = server.getHostports();
	if (!_hostports.empty())
	{
		stream << "| HOST PORTS\t:";
		for (std::vector<hostport>::const_iterator it = _hostports.begin(); it != _hostports.end(); it++)	
			stream << "\n  - [" << (!it->host.empty() ? it->host + ":" : "") << it->port << "]" << (it->default_server ? " default_server" : "");
		stream << "\n";
	}
	stream << server.getVLocation();
	return (stream); 
}

/*--------------------------------------------------------------*/
/*								GETERS							*/
/*--------------------------------------------------------------*/

std::string				Server::getServer_name() const {return server_name;}
std::vector<hostport>	Server::getHostports() const {return hostports;}
Location				&Server::getVLocation() {return vLocation;}
std::list<int>			&Server::getSockets() {return sockets;}
