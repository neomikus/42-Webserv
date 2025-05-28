#include <iostream>

using namespace std;

struct cgi
{ };

struct error_page
{
	int		*error_codes;
	int		reponse_code;
	string	page;
};

struct location
{
	string		name;
	string		*allowed_methods;
	location	*recursivity;
	string		root;
	bool		directory_listing;
	string		root;
	string		*index;
	cgi			*cgi;
};

struct file_t
{ }; 


class server
{
private:

	string			*name;
	string			host;
	int				port;
	bool			default_server;
	error_page		*error_pages;
	int 			max_body_size;
	file_t			*files;
	bool			directory_listing;
	string			root;
	string			*index;


public:

};