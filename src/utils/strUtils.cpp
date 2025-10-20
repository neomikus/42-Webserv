#include "webserv.hpp"

std::string cgiToStr(int choice)
{
	switch (choice)
	{
	case 0:
		return std::string("BASH");
	case 1:
		return std::string("PHP");
	case 2:
		return std::string("PYTHON");
	case 3:
		return std::string("GO");
	case 4:
		return std::string("NONE");
	default:
		break;
	}
	return("");
}


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

std::string strTrim(std::string str) {return(rtrim(ltrim(str))); }



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

size_t	cstrlen(const char *str) {
	int size = 0;
	while (str[size])
		size++;
	return (size);
}

size_t	cstrcat(char *dst, const char *src) {
	if (!dst || !src)
		return (0);
		
	int j = cstrlen(dst);
	for (size_t i = 0; src[i]; i++) {
		dst[j + i] = src[i];
	}

	size_t size = cstrlen(src) + j;
	dst[size] = '\0';
	return (size);
}

std::string makeString(const std::vector<char> vec) {
	std::string	retval;
	for (std::vector<char>::const_iterator it = vec.begin(); it != vec.end() ; ++it) {
		retval += *it;
	}
	return (retval);
}

std::string makeString(std::vector<char>::iterator start, std::vector<char>::iterator end) {
	std::string	retval;

	for (std::vector<char>::iterator it = start; it != end ; ++it) {
		retval += *it;
	}
	return (retval);
}

char *makeCString(const std::vector<char> vec) {
	char	*retval = new char[vec.size()];

	size_t i = 0;
	for (std::vector<char>::const_iterator it = vec.begin(); it != vec.end() ; ++it) {
		retval[i++] = *it;
	}

	retval[i] = '\0';
	return (retval);
}

char *makeCString(std::vector<char>::iterator start, std::vector<char>::iterator end) {
	char	*retval = new char[std::distance(start, end)];
	size_t i = 0;
	for (std::vector<char>::iterator it = start; it != end ; ++it) {
		retval[i++] = *it;
	}

	retval[i] = '\0';
	return (retval);
}