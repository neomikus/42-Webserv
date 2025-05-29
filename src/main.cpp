#include "webserv.hpp"


Server context_server(std::ifstream &confFile)
{

	
	return newServer;
}
location context_location()
{
	location loca;
	return loca;
}

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
			servers.push_back(context_server(confFile));
	}
	return (0);
}