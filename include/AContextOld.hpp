#ifndef ACONTEXT_HPP
#define ACONTEXT_HPP

#include "webserv.hpp"


class Context {
	protected:
		
	std::string					root;
	std::vector<std::string>	index;
	bool						autoindex;
	allowed_methods				methods;
	std::vector<error_page>		error_pages;
	
	
	void					parseRoot(std::string value);
	void					parseIndex(std::string value);
	void					parseAutoindex(std::string value);
	void					parseAlowedMethods(std::string value);
	void					parseErrorPage(std::string value);

	public:
	
		Context();
		virtual ~Context();
		
		std::string					getRoot() const;
		std::vector<std::string>	getIndex() const;
		bool						getAutoindex() const;
		allowed_methods				getMethods() const;
		std::vector<error_page>		getError_pages() const;

};

#endif