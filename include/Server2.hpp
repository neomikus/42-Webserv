#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.hpp"
#include "AContext.hpp"
#include "Location2.hpp"

class Server {
	private:
		std::list<int>				sockets;
		std::string					server_name;
		std::vector<hostport>		hostports;
		Location					vLocation;

		void						parseServerName(std::string value);
		void						parseHostPort(std::string value);
	
	public:
	
		Server();
		Server(std::ifstream &confFile);
		Server(const Server &model);
		Server&operator=(const Server&model);
		~Server();

		std::string				getServer_name() const;
		std::vector<hostport>	getHostports() const;
		Location				&getVLocation();
		std::list<int>			&getSockets();

};

std::ostream &operator<<(std::ostream &stream, Server server);

void		acceptConnections(int epfd, std::vector<Server> &servers);

#endif