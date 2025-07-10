#include "webserv.hpp"
#include "Server.hpp"

int	main(int argc, char *argv[]) {

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
	}

	std::cout << servers.front() << std::endl;

	return (0);
}