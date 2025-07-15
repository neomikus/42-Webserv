#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.hpp"
#include "AContext.hpp"
#include "Location.hpp"

class Server: public Context {
	private:
		std::list<int>				sockets;
		std::string					server_name;
		std::vector<hostport>		hostports;
		long long					max_body_size;
		std::vector<Location>		locations;

		void						parseServerName(std::string value);
		void						parseHostPort(std::string value);
		void						parseBodySize(std::string value);
	
	public:
	
		Server();
		Server(std::ifstream &confFile);
		Server(const Server &model);
		Server&operator=(const Server&model);
		~Server();

		std::string				getServer_name() const;
		std::vector<hostport>	getHostports() const;
		long long				getMax_body_size() const;
		std::vector<Location>	&getLocations();
		std::list<int>			&getSockets();

};

std::ostream &operator<<(std::ostream &stream, Server server);

void		acceptConnections(int epfd, std::vector<Server> &servers);

#endif