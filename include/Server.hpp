#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.hpp"
#include "AContext.hpp"
#include "Location.hpp"

class Server: public Context {
	private:
		
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
		std::string	displayConf() const;

};

std::ostream &operator<<(std::ostream &stream, const Server server);

#endif