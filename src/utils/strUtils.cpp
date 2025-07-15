#include "webserv.hpp"


size_t countWords(std::stringstream& ss) {
    size_t count = 0;
    std::string word;
    while (ss >> word) {
        count++;
    }
    ss.clear();
    ss.seekg(0);
    return count;
}

size_t countWords(std::string const str) {
	std::stringstream ss(str);
    return countWords(ss);
}


bool strIsDigit(std::string const str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
		if (!std::isdigit(*it))
			return (false);
	return(true);
}


std::string ltrim(std::string str)
{
	for (std::string::iterator it = str.begin(); it != str.end(); ++it)
	{
		if (*it != ' ' && *it != '\t')
		{
			str.erase(str.begin(), it);
			return str;
		}
	}
	return std::string("");
}
std::string rtrim(std::string str)
{
	for (std::string::iterator it = str.end() - 1; it != str.begin() - 1; --it)
	{
		if (*it != ' ' && *it != '\t')
		{
			str.erase(it + 1, str.end());
			return str;
		}
	}
	return std::string("");
}

std::string strTrim(std::string str) {return(rtrim(ltrim(str))); 
}



std::string ltrim(std::string str, char delimiter)
{
	for (std::string::iterator it = str.begin(); it != str.end(); ++it)
	{
		if (*it != delimiter)
		{
			str.erase(str.begin(), it);
			return str;
		}
	}
	return std::string("");
}
std::string rtrim(std::string str, char delimiter)
{
	for (std::string::iterator it = str.end() - 1; it != str.begin() - 1; --it)
	{
		if (*it != delimiter)
		{
			str.erase(it + 1, str.end());
			return str;
		}
	}
	return std::string("");
}

std::string strTrim(std::string str, char delimiter) {return(rtrim(ltrim(str, delimiter), delimiter)); 
}

std::string trimLastWord(std::string str, char delimiter)
{
	for (std::string::iterator it = str.end() - 1; it != str.begin() - 1; --it)
	{
		if (*it == delimiter)
		{
			str.erase(it, str.end());
			if (str.empty())
				return std::string("/");
			return (str);
		}
	}
	return std::string("");
}

std::vector<std::string> strSplit(const std::string& str, const std::string& delimiter) {
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = str.find(delimiter);
	
	while (end != std::string::npos) {
		tokens.push_back(str.substr(start, end - start));
		start = end + delimiter.length();
		end = str.find(delimiter, start);
	}
	
	tokens.push_back(str.substr(start));
    
	return tokens;
}