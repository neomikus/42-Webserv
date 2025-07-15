#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "webserv.hpp"
#include "AContext.hpp"

class Location {
	private:

		std::string					uri;
		long long					level;
		cgi_options					cgi;
		std::vector<Location>		locations;
		std::string					root;
		std::vector<std::string>	index;
		bool						autoindex;
		allowed_methods				methods;
		std::vector<error_page>		error_pages;
		long long					max_body_size;

		void						parseCgi(std::string value);
		void						parseRoot(std::string value);
		void						parseIndex(std::string value);
		void						parseAutoindex(std::string value);
		void						parseAlowedMethods(std::string value);
		void						parseErrorPage(std::string value);
		void						parseBodySize(std::string value);


		public:

		Location();
		Location(std::string value, std::ifstream &confFile, int nest);
		Location(const Location &model);
		Location &operator=(const Location &model);
		~Location();

		std::string					getUri() const;
		long long					getLevel() const;
		cgi_options					getCgi() const;
		std::vector<Location>		&getLocations();
		std::string					getRoot() const;
		std::vector<std::string>	getIndex() const;
		bool						getAutoindex() const;
		allowed_methods				getMethods() const;
		std::vector<error_page>		getError_pages() const;
		long long					getMax_body_size() const;

};

std::ostream &operator<<(std::ostream &stream, Location location);

#endif