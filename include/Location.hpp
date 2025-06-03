#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Webserv.hpp"

class Location {
	private:

		std::string					uri;
		std::string					root;
		std::vector<std::string>	index;
		bool						autoindex;
		std::vector<error_page>		error_pages;
		cgi_options					cgi;
		std::vector<Location*>		locations;
		int							level;

	public:

		Location();
		Location(std::string value, std::ifstream &confFile, int nest);
		~Location();
		std::string	displayConf() const;

};

std::ostream &operator<<(std::ostream &stream, const Location location);

#endif