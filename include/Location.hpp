#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "webserv.hpp"
#include "AContext.hpp"

class Location: public Context {
	private:

		std::string					uri;
		long long					level;
		cgi_options					cgi;
		std::vector<Location>		locations;

		void						parseCgi(std::string value);

		public:

		Location();
		Location(std::string value, std::ifstream &confFile, int nest);
		Location(const Location &model);
		Location &operator=(const Location &model);
		~Location();

		std::string				getUri() const;
		long long				getLevel() const;
		cgi_options				getCgi() const;
		std::vector<Location>	getLocations() const;

};

std::ostream &operator<<(std::ostream &stream, const Location location);

#endif