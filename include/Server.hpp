#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"
#include "Location.hpp"

class Server {
	private:
		
		std::string					server_name;
		std::vector<hostport>		hostports;
		std::vector<error_page>		error_pages;
		long long					max_body_size;
		bool						autoindex;
		std::string					root;
		std::vector<std::string>	index;
		std::vector<Location>		locations;
	
	public:
	
		Server();
		Server(std::ifstream &confFile);
		~Server();
		std::string	displayConf() const;

};

std::ostream &operator<<(std::ostream &stream, const Server server);

#endif