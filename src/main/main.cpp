#include "webserv.hpp"
#include "Server.hpp"
#include "Request.hpp"

bool		sigstop = false;
int			errorCode = 0;
std::string	errorLine = "";

void stop(int sig) {
	if (sig == SIGINT) {
		sigstop = true;
	}
}

int	errorMesage(std::string fileName)
{
	std::cerr << "Error in " << fileName << " : ";
	if (errorCode == 3)
		std::cerr << "parsing error" << std::endl;
	std::cerr << errorLine << std::endl;
	return (errorCode);
}

int	main(int argc, char *argv[]) {
	signal(SIGINT, stop);

	if (argc < 2)
	{
		std::cout << "file not given" << std::endl;
		return (1);
	}

	
	std::vector<Server> servers;
	
	std::ifstream		confFile(argv[1]);
	
	if (!confFile.is_open())
	{
		std::cout << "file not found" << std::endl;
		return (1);
	}
	
	for (std::string buffer; std::getline(confFile, buffer);)
	{
		buffer = strTrim(buffer);
		if (buffer.empty())
		continue;
		if (buffer == "server {")
			servers.push_back(Server(confFile));
		if (errorCode != 0)
			break;
	}
	if (errorCode != 0)
		return (errorMesage(argv[1]));
	
	int	epfd = epoll_create(1);
	
	std::cout << servers.front() << std::endl;

 	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
		std::vector<hostport> _hostport = it->getHostports();
		for (std::vector<hostport>::iterator it2 = _hostport.begin(); it2 != _hostport.end(); ++it2) {
			// Change inet_addr to something allowed by the subject later
			it->getSockets().push_back(Socket::initServer(it2->port, inet_addr(it2->host.c_str()), epfd));
		}
	}

	acceptConnections(epfd, servers);
	return (0);
}