#include "webserv.hpp"

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

	for (std::string buffer; !confFile.eof(); std::getline(confFile, buffer))
	{
		if (buffer == "server {")
			servers.push_back(Server(confFile));
	}
	std::cout << servers.front().displayConf() << std::endl;
	return (0);
}