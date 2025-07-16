#include "Location.hpp"

Location::Location() {
	uri = "";
	level = 0;
	cgi = NONE;
	root = "";
	autoindex = false;
	allowed_methods a_methods = {false, false, false};
	this->methods = a_methods;
	max_body_size = MB;

}

Location::~Location(){

}

Location::Location(const Location& model) {
    uri = model.uri;
    root = model.root;
    index = model.index;
    autoindex = model.autoindex;
    error_pages = model.error_pages;
    cgi = model.cgi;
    level = model.level;
    methods = model.methods;
	max_body_size = model.max_body_size;
    for (size_t i = 0; i < model.locations.size(); i++) {
        locations.push_back(Location(model.locations[i]));
    }
}

Location &Location::operator=(const Location &model) {

	uri = model.uri;
	root = model.root;
	index = model.index;
	autoindex = model.autoindex;
	error_pages = model.error_pages;
	cgi = model.cgi;
	level = model.level;
	methods = model.methods;
	max_body_size = model.max_body_size;
	for (size_t i = 0; i < model.locations.size(); i++) {
		locations.push_back(Location(model.locations[i]));
	}
	return(*this);
}

void	Location::parseCgi(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;
	const std::string	cgiStr[5] = {"BASH", "PHP", "PYTHON", "GO", "NONE"};

	for (size_t i = 0; i < 5; i++)
	{
		if (value == cgiStr[i])
		{
			cgi = (static_cast<cgi_options>(i));
			return;
		}
	}
	exit (0);
}
void	Location::parseErrorPage(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;

	if (value.empty() || value.find('/') == value.npos)
	{
		std::cout << HRED << "ERROR ERROR PAGE1" << std::endl;
		exit(0);
	}
	
	error_page	error_page;
	error_page.page = "";
	error_page.to_replace = 0;

	std::string	strPage;
	std::string	redirect;
	std::string	error_codes;

	strPage = strTrim(value.substr(value.find('/') + 1));

	value = strTrim(value.substr(0, value.find('/')));

	if (strPage.find(' ') != strPage.npos)
	{
		std::cout << HRED << "ERROR ERROR PAGE2" << std::endl;
		exit(0);
	}

	if (value.find('=') != value.npos)
	{
		error_codes = strTrim(value.substr(0, value.find('=')));
		redirect = strTrim(value.substr(value.find('=') + 1));	}
	else
	{
		error_codes = value;
		error_page.to_replace = -1;
	}

	if (error_codes.empty() || (!redirect.empty() && !strIsDigit(redirect)))
	{
		std::cout << HRED << "ERROR ERROR PAGE3" << std::endl;
		exit(0);
	}

	if (error_codes.find(' ') != error_codes.npos)
	{
		std::stringstream	split(error_codes);
		while (split)
		{
			std::string			error_c;
			split >> error_c;
			if(error_c.empty())
				break ;
			if (!strIsDigit(error_c))
				exit (0);
			error_page.to_catch.push_back(atoi(error_c.c_str()));
		}
	}
	else if (strIsDigit(error_codes))
		error_page.to_catch.push_back(atoi(error_codes.c_str()));
	else
		exit (0);

	error_page.page = strPage;
	if (error_page.to_replace != -1)
		error_page.to_replace = atoi(redirect.c_str());

	error_pages.push_back(error_page);
}

void	Location::parseAutoindex(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;
	if (value == "on")
		autoindex = true;
	else if (value == "off")
		autoindex = false;
	else
	{
		std::cout << HRED << "autoindex on or off" << std::endl;
		exit(0);
	}
}

void	Location::parseRoot(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;
	if (value.find(' ') != value.npos || value.find('\t') != value.npos)
		exit (0);
	root = value;
}

void	Location::parseIndex(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;
	std::vector<std::string> indexVector;

	std::stringstream	split(value);
	while (split)
	{
		std::string			tmp_index;
		split >> tmp_index;
		if(tmp_index.empty())
			break ;
		indexVector.push_back(tmp_index);
	}
	this->index = indexVector;
}

void Location::parseBodySize(const std::string value) {
	//std::cout << "[" << value << "]" << std::endl;
	size_t i = 0;

	std::map<std::string, int> unitMap;
	unitMap["b"] = 0;
	unitMap["k"] = 1;
	unitMap["kb"] = 1;
	unitMap["m"] = 2;
	unitMap["mb"] = 2;
	unitMap["g"] = 3;
	unitMap["gb"] = 3;

	long long multipliers[] = {
		BYTE, KB, MB, GB
	};

	while (i < value.size() && std::isdigit(value[i]))
		++i;

	std::string unit = value.substr(i);

	long long size = atoll(value.substr(0, i).c_str());

	if (i == 0)
		exit (0);
	
	for (i = 0; i < unit.size(); ++i) {
		unit[i] = std::tolower(unit[i]);
	}

	if (unitMap.find(unit) == unitMap.end())
		exit (0);
	
	this->max_body_size = (size * multipliers[unitMap.find(unit)->second]);
}

void Location::parseAlowedMethods(std::string value){
	//std::cout << "[" << value << "]" << std::endl;

	std::stringstream	split(value);
	allowed_methods a_methods = {false, false, false};
	this->methods = a_methods;
	while (split)
	{
		std::string			word;
		split >> word;

		if (word.empty())
			break;

		if (word == "GET")
			methods._get = true;
		else if (word == "POST")
			methods._post = true;
		else if (word == "DELETE")
			methods._delete = true;
		else
		{
			std::cout << "method not implemented" << std::endl;
			exit(0);
		}
	}

}

Location::Location(std::string value, std::ifstream &confFile, int nest, const Location father)
{
	std::cout << "[" << value << "]" << std::endl;
	this->methods = father.getMethods();
	this->max_body_size = father.getMax_body_size();
	this->error_pages = father.getError_pages();
	this->root = father.getRoot();
	this->autoindex = father.getAutoindex();

	if (nest != 0 && (value.empty() || value.find('{') == value.npos))
	{
		std::cout << HRED << "ERROR1" << std::endl;
		exit(0);
	}

	value = strTrim(value.substr(0, value.find('{')));

	if (nest != 0 && (value.empty() || value.find(' ') != value.npos || value.find('\t') != value.npos))
	{
		std::cout << HRED << "ERROR2" << std::endl;
		exit(0);
	}

	level = nest;
	uri = value;
	cgi = NONE;
	autoindex = false;
	
	std::string key_words[10] = {
	"error_page", "location", "autoindex", "root", "index", "cgi", "allowed_methods", "client_max_body_size", "listen", "server_name"};
	for (std::string buffer; std::getline(confFile, buffer);)
	{
		buffer = strTrim(buffer);
		if (buffer.empty() || buffer == ";" || buffer[0] == '#')
			continue;
		if (buffer == "}")
			return ;
		if (buffer.find(';') == buffer.npos && buffer.find("location") == buffer.npos)
		{
			std::cout << "ERROR3" << std::endl;
			exit(0);
		}
		int key;
		for (key = 0; key < 9; key++)
			if (buffer.substr(0, buffer.find_first_of(' ')) == key_words[key])
				break;

		if ((key == 8 || key == 9) && nest != 0)
		{
			std::cout << "ERROR5" << std::endl;
			exit(0);
		}
		std::cout << "choosed key = " << key << std::endl;
		std::cout << buffer << std::endl;
		
		value = buffer.substr(key_words[key].size(), buffer.find_first_of(';') - key_words[key].size());
		value = strTrim(value);
		switch (key)
		{
			case 0:
				parseErrorPage(value);
				break;
			case 1:
				locations.push_back(Location(value, confFile, level + 1, *this));
				break;
			case 2:
				parseAutoindex((value));
				break;
			case 3:
				parseRoot(value);
				break;
			case 4:
				parseIndex(value);
				break;
			case 5:
				parseCgi(value);
				break;
			case 6:
				parseAlowedMethods(value);
				break;
			case 7:
				parseBodySize(value);
				break;
			default:
				break;
		}
	}
}

std::ostream &operator<<(std::ostream &stream, Location location) {
	
	const char *colors[4] = {HBLU, HGRE, HMAG, HRED};
	std::string	cgiStr[5] = {"BASH", "PHP", "PYTHON", "GO", "NONE"};
	std::string tabs("");
	if (location.getLevel() != 0)
	{
		tabs = std::string(location.getLevel(), '\t');
		stream << colors[location.getLevel() % 4];
		stream << std::string(location.getLevel() - 1, '\t') << "LOCATION\t: " << location.getUri() << std::endl;

	}
	//stream << tabs << "| URI\t\t: " << location.getUri() << "\n";

	if (!location.getRoot().empty())
		stream << tabs << "| ROOT\t\t: " << location.getRoot() << "\n";

	if (location.getAutoindex())
	stream << tabs << "| AUTOINDEX\t: ON" << "\n";

	if (location.getCgi() != NONE)
	stream << tabs << "| CGI\t\t: " << cgiStr[location.getCgi()]<< "\n";

	std::vector<std::string> _index = location.getIndex();
	if (!_index.empty())
	{
		stream << tabs << "| INDEX\t\t:";
		for (std::vector<std::string>::const_iterator it = _index.begin(); it != _index.end(); ++it)
			stream << "\n" << tabs << "  - [" << *it << "]";
		stream << tabs << "\n";
	}

	std::vector<error_page> _error_pages = location.getError_pages();
	if (!_error_pages.empty())
	{
		stream << tabs << "| ERROR PAGES\t:";
		for (std::vector<error_page>::const_iterator it = _error_pages.begin(); it != _error_pages.end(); ++it)
		{
			stream << "\n" << tabs << "  - [";
			for (std::vector<int>::const_iterator it_catch = it->to_catch.begin(); it_catch != it->to_catch.end(); ++it_catch)
				stream << *it_catch << " ";
			if (it->to_replace != -1)
				stream << "= " << it->to_replace;
			stream << " \\ " << it->page << "]";
		}
		stream << "\n";
	}

	if (location.getMethods()._delete || location.getMethods()._get || location.getMethods()._post)
	{
		stream << tabs << "| METHODS\t:";
		stream << (location.getMethods()._get ? " GET" : "");
		stream << (location.getMethods()._post ? " POST" : "");
		stream << (location.getMethods()._delete ? " DELETE" : "");
		stream << "\n";
	}

	stream << tabs <<  "| MAX BODY SIZE\t: " << location.getMax_body_size() <<  + "\n";

	std::vector<Location> _locations = location.getLocations();
	if (!_locations.empty())
	{
		stream << tabs << "| LOCATIONS\t:";
		stream << "\n";
		for (std::vector<Location>::const_iterator it = _locations.begin(); it != _locations.end(); ++it)
			stream << *it;
	}
	return(stream);
}


/*--------------------------------------------------------------*/
/*								GETERS							*/
/*--------------------------------------------------------------*/

std::string					Location::getUri() const {return uri;}
long long					Location::getLevel() const {return level;}
cgi_options					Location::getCgi() const {return cgi;}
std::vector<Location>		&Location::getLocations() {return locations;}
std::string					Location::getRoot() const {return root;}
std::vector<std::string>	Location::getIndex() const {return index;}
bool						Location::getAutoindex() const {return autoindex;}
allowed_methods				Location::getMethods() const {return methods;}
std::vector<error_page>		Location::getError_pages() const {return error_pages;}
long long					Location::getMax_body_size() const {return max_body_size;}