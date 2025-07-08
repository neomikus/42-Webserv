#include "webserv.hpp"
#include "Server.hpp"

int	main(int argc, char *argv[]) {

	if (argc < 2)
	{
		std::cout << "file not given" << std::endl;
		return (1);
	}

	int	epfd = epoll_create(1);

	std::vector<Server> servers;

	std::ifstream		confFile(argv[1]);

	if (!confFile.is_open())
	{
		std::cout << "file not found" << std::endl;
		return (1);
	}

	for (std::string buffer; std::getline(confFile, buffer);)
	{
		if (buffer.empty())
			continue;
		if (buffer == "server {")
			servers.push_back(Server(confFile));
	}

	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++) {
		for (std::vector<hostport>::iterator it2 = it->hostports.begin(); it2 != it->hostports.end(); it2++) {
			// Change inet_addr to something allowed by the subject later
			it->sockets.push_back(Socket::initServer(it2->second, inet_addr(it2->first.c_str()), epfd));
		}
	}

	acceptConnections(epfd, servers);

	return (0);
}  
