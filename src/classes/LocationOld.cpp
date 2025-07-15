#include "Location.hpp"

Location::Location() { }

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
	for (size_t i = 0; i < model.locations.size(); i++) {
		locations.push_back(Location(model.locations[i]));
	}
	return(*this);
}

void	Location::parseCgi(std::string value) {
	//std::cout << "[" << value << "]" << std::endl;
	std::string	cgiStr[5] = {"BASH", "PHP", "PYTHON", "GO", "NONE"};

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

Location::Location(std::string value, std::ifstream &confFile, int nest)
{
	//std::cout << "[" << value << "]" << std::endl;

	if (value.empty() || value.find('{') == value.npos)
	{
		std::cout << "ERROR1" << std::endl;
		exit(0);
	}

	value = strTrim(value.substr(0, value.find('{')));

	if (value.empty() || value.find(' ') != value.npos || value.find('\t') != value.npos)
	{
		std::cout << "ERROR2" << std::endl;
		exit(0);
	}

	level = nest;
	uri = value;
	cgi = NONE;
	autoindex = false;

	std::string key_words[8] = {
	"error_page", "location", "autoindex", "root", "index", "cgi", "allow_methods"};
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
		for (key = 0; key < 8; key++)
			if (buffer.substr(0, buffer.find_first_of(' ')) == key_words[key])
				break;
		value = buffer.substr(key_words[key].size(), buffer.find_first_of(';') - key_words[key].size());
		value = strTrim(value);
		switch (key)
		{
			case 0:
				parseErrorPage(value);
				break;
			case 1:
				locations.push_back(Location(value, confFile, level + 1));
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
			default:
				break;
				
		}
	}
}

std::ostream &operator<<(std::ostream &stream, Location location) {
	
	std::string tabs(location.getLevel(), '\t');
	const char *colors[4] = {HBLU, HGRE, HMAG, HRED};
	std::string	cgiStr[5] = {"BASH", "PHP", "PYTHON", "GO", "NONE"};

	stream << colors[location.getLevel() % 4];

	stream << std::string(location.getLevel() - 1, '\t') << "LOCATION:" << std::endl;

	stream << tabs << "| URI\t\t: " << location.getUri() << "\n";

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

std::string				Location::getUri() const {return uri;}
long long				Location::getLevel() const {return level;}
cgi_options				Location::getCgi() const {return cgi;}
std::vector<Location>	&Location::getLocations() {return locations;}