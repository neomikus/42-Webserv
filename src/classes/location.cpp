#include "Location.hpp"

Location::Location() { }

Location::~Location(){ }


Location::Location(std::string value, std::ifstream &confFile, int nest)
{
	//std::cout << "[" << value << "]" << std::endl;

	if (value.empty() || value.find('{') == value.npos)
	{
		std::cout << "ERROR" << std::endl;
		exit(0);
	}

	value = strTrim(value.substr(0, value.find('{')));

	if (value.empty() || value.find(' ') != value.npos || value.find('\t') != value.npos)
	{
		std::cout << "ERROR" << std::endl;
		exit(0);
	}

	level = nest;
	uri = value;
	cgi = NONE;
	autoindex = false;

	std::string key_words[8] = {
	"error_page", "location", "autoindex", "root", "index", "cgi"};
	for (std::string buffer; !confFile.eof(); std::getline(confFile, buffer))
	{
		buffer = strTrim(buffer);
		if (buffer.empty() || buffer == ";" || buffer[0] == '#')
			continue;
		if (buffer == "}")
			return ;
		if (buffer.find(';') == buffer.npos && buffer.find("location") == buffer.npos)
		{
			std::cout << "ERROR" << std::endl;
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
				error_pages.push_back(parseErrorPage(value));
				break;
			case 1:
			locations.push_back(new Location(value, confFile, level + 1));
				break;
			case 2:
				autoindex = parseAutoindex((value));
				break;
			case 3:
				root = parseRoot(value);
				break;
			case 4:
				index = parseIndex(value);
				break;
			case 5:
				cgi = parseCgi(value);
				break;
			default:
				break;
		}		
	}
}
std::string Location::displayConf() const {
	std::stringstream strConf;
	std::string tabs(level, '\t');
	const char *colors[4] = {HBLU, HGRE, HMAG, HRED};
	std::string	cgiStr[5] = {"BASH", "PHP", "PYTHON", "GO", "NONE"};

	strConf << colors[level % 4];

	strConf << std::string(level - 1, '\t') << "LOCATION:" << std::endl;

	strConf << tabs << "| URI\t\t: " << uri << "\n";

	if (!root.empty())
		strConf << tabs << "| ROOT\t\t: " << root << "\n";

	if (autoindex)
	strConf << tabs << "| AUTOINDEX\t: ON" << "\n";

	if (cgi != NONE)
	strConf << tabs << "| CGI\t\t: " << cgiStr[cgi]<< "\n";

	if (!index.empty())
	{
		strConf << tabs << "| INDEX\t\t:";
		for (std::vector<std::string>::const_iterator it = index.begin(); it != index.end(); it++)
			strConf << "\n" << tabs << "  - [" << *it << "]";
		strConf << tabs << "\n";
	}

	if (!error_pages.empty())
	{
		strConf << tabs << "| ERROR PAGES\t:";
		for (std::vector<error_page>::const_iterator it = error_pages.begin(); it != error_pages.end(); it++)
		{
			strConf << "\n" << tabs << "  - [";
			for (std::vector<int>::const_iterator it_catch = it->to_catch.begin(); it_catch != it->to_catch.end(); it_catch++)
				strConf << *it_catch << " ";
			if (it->to_replace != -1)
				strConf << "= " << it->to_replace;
			strConf << " \\ " << it->page << "]";
		}
		strConf << "\n";
	}
	if (!locations.empty())
	{
		strConf << tabs << "| LOCATIONS\t:";
		strConf << "\n";
		for (std::vector<Location*>::const_iterator it = locations.begin(); it != locations.end(); it++)
			strConf << **it;
	}
	return (strConf.str());
}

std::ostream &operator<<(std::ostream &stream, const Location location) {
	stream << location.displayConf();
	return(stream);
	
}
