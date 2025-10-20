#include "webserv.hpp"
#include "Server.hpp"
#include "Request.hpp"

bool		sigstop = false;
int			errorCode = 0;
std::string	errorLine = "";
char **global_envp = NULL;

void stop(int sig) {
	if (sig == SIGINT) {
		sigstop = true;
	}
}

int	errorMesage(std::string fileName)
{
	std::cerr << "Error in " << fileName << " : ";
	switch (errorCode) {
		case 1:
			std::cerr << "something found outside context" << std::endl;
			break;
		case 2:
			std::cerr << "semicolom not found" << std::endl;
			break;
		case 3:
			std::cerr << "\"server {\" dosnt recive arguments" << std::endl;
			break;
		case 4:
			std::cerr << "<value> empty" << std::endl;
			break;
		case 5:
			std::cerr << "location must open the braket in the same line" << std::endl;
			break;
		case 6:
			std::cerr << "location must have a <value> and a {" << std::endl;
			break;
		case 7:
			std::cerr << "server context keyword found in location context" << std::endl;
			break;
		case 8:
			std::cerr << "error_page needs an error_page to be redirected." << std::endl;
			break;
		case 9:
			std::cerr << "the error_page redirect page is only one argument" << std::endl;
			break;
		case 10:
			std::cerr << "error_page error code must be a digit" << std::endl;
			break;
		case 11:
			std::cerr << "Autoindex argument must be \"on\" or \"off\"" << std::endl;
			break;
		case 12:
			std::cerr << "client_max_body_size argument must be an integer followed by the unit" << std::endl;
			break;
		case 13:
			std::cerr << "client_max_body_size unit goes from b to Gb" << std::endl;
			break;
		case 14:
			std::cerr << "allowed methods are \"GET\", \"POST\" and \"DELETE\"" << std::endl;
			break;
		default:
			break;
	}
	std::cerr << errorLine << std::endl;
	return (errorCode);
}

int	main(int argc, char *argv[], char *envp[]) {
	signal(SIGINT, stop);
	global_envp = envp;
	if (argc < 2)
	{
		std::cerr << "file not given" << std::endl;
		return (1);
	}

	std::vector<Server> servers;
	
	std::ifstream		confFile(argv[1]);
	
	if (!confFile.is_open())
	{
		std::cerr << "file not found" << std::endl;
		return (1);
	}
	
	for (std::string buffer; std::getline(confFile, buffer);)
	{
		buffer = strTrim(buffer);
		if (buffer.empty())
			continue;
		std::vector<std::string> split = strSplit(buffer, " ");
		if (split[0] == "server" && split[1] == "{") {
			if (split.size() > 2)
			{
				errorCode = 3;
				errorLine = buffer;
				break;
			}
			servers.push_back(Server(confFile));
		}
		else {
			errorCode = 1;
			errorLine = buffer;
			break;
		}
		if (errorCode != 0)
			break;
	}
	if (errorCode != 0)
		return (errorMesage(argv[1]));
	
	int	epfd = epoll_create(1);

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