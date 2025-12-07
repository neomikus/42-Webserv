#include "webserv.hpp"

// Can't use it at 42
/*
#include <magic.h>
#define MAGIC_DATABASE "/usr/share/file/misc/magic.mgc"

std::string	getMIME(std::string filename) {
	std::string	retval("application/octet-stream");
	magic_t	magic = magic_open(MAGIC_MIME_TYPE);
	magic_load(magic, MAGIC_DATABASE);
	const char *mime = magic_file(magic, filename.c_str());
	if (mime)
		retval = mime;
	magic_close(magic);
	return (retval);
}
*/

// MY QUEST FOR MIME TYPES IN C++ STARTS NOW!!!

#define MIME_PATH "/usr/share/mime/"

std::map<std::string, std::string> mime_init() {
	std::vector<std::string>	types;
	types.push_back("text");
	types.push_back("audio");
	types.push_back("image");
	types.push_back("video");
	types.push_back("application");

	std::map<std::string, std::string>	mimeLib;
	
	for (std::vector<std::string>::iterator it = types.begin(); it != types.end(); ++it) {
		DIR		*mimeDir = opendir((MIME_PATH + *it).c_str());
		
		for (dirent *current = readdir(mimeDir); current; current = readdir(mimeDir)) {
			std::ifstream	currentFile;
			currentFile.open((std::string(MIME_PATH) + *it + "/" + std::string(current->d_name)).c_str());
			std::string		type;

			for (std::string line; std::getline(currentFile, line);) {
				if (line.find("<mime-type") != line.npos) {
					std::vector<std::string> words = strSplit(line, " ");
					for (std::vector<std::string>::iterator wordIt = words.begin(); wordIt != words.end(); ++wordIt) {
						if (wordIt->find("type=") != wordIt->npos) {
							type = wordIt->substr(wordIt->find("type=\"") + cstrlen("type=\""), wordIt->find_last_of("\"") - cstrlen("type=\""));
							break;
						}
					}
				} else if (line.find("<glob") != line.npos) {
					std::vector<std::string> words = strSplit(line, " ");
					for (std::vector<std::string>::iterator wordIt = words.begin(); wordIt != words.end(); ++wordIt) {
						if (wordIt->find("pattern=") != wordIt->npos) {
							std::string	pattern = wordIt->substr(wordIt->find("pattern=\"*") + cstrlen("pattern=\"*"), wordIt->find_last_of("\"") - cstrlen("pattern=\"*"));
							if (mimeLib.find(pattern) == mimeLib.end()) {
								std::pair<std::string, std::string>	pair = std::make_pair(pattern, type);
								mimeLib.insert(pair);
							}
							break;
						}
					}			
				}
			}
			currentFile.close();
		}		
		
		closedir(mimeDir);
	}
	return (mimeLib);
}

std::map<std::string, std::string> mime_reverse_init() {
	std::vector<std::string>	types;
	types.push_back("text");
	types.push_back("audio");
	types.push_back("image");
	types.push_back("video");
	types.push_back("application");

	std::map<std::string, std::string>	mimeLib;
	
	for (std::vector<std::string>::iterator it = types.begin(); it != types.end(); ++it) {
		DIR		*mimeDir = opendir((MIME_PATH + *it).c_str());
		
		for (dirent *current = readdir(mimeDir); current; current = readdir(mimeDir)) {
			std::ifstream	currentFile;
			currentFile.open((std::string(MIME_PATH) + *it + "/" + std::string(current->d_name)).c_str());
			std::string		type;

			for (std::string line; std::getline(currentFile, line);) {
				if (line.find("<mime-type") != line.npos) {
					std::vector<std::string> words = strSplit(line, " ");
					for (std::vector<std::string>::iterator wordIt = words.begin(); wordIt != words.end(); ++wordIt) {
						if (wordIt->find("type=") != wordIt->npos) {
							type = wordIt->substr(wordIt->find("type=\"") + cstrlen("type=\""), wordIt->find_last_of("\"") - cstrlen("type=\""));
							break;
						}
					}
				} else if (line.find("<glob") != line.npos) {
					std::vector<std::string> words = strSplit(line, " ");
					for (std::vector<std::string>::iterator wordIt = words.begin(); wordIt != words.end(); ++wordIt) {
						if (wordIt->find("pattern=") != wordIt->npos) {
							std::string	pattern = wordIt->substr(wordIt->find("pattern=\"*") + cstrlen("pattern=\"*"), wordIt->find_last_of("\"") - cstrlen("pattern=\"*"));
							if (mimeLib.find(pattern) == mimeLib.end()) {
								std::pair<std::string, std::string>	pair = std::make_pair(type, pattern);
								mimeLib.insert(pair);
							}
							break;
						}
					}			
				}
			}
			currentFile.close();
		}		
		closedir(mimeDir);
	}
	return (mimeLib);
}

/* Searches for needle in mimeLib or mimeRLib depending on the flag reverse. If mimeLib and mimeRLib are not intialized, it initializes both, no matter the reverse value

If !reverse, it searches a MIME type/subtype based on a extension. If not found returns "application/octet-stream"
If reverse, it searches an extension based on a MIME type/subtype. If not found returns empty string
*/
std::string	getMIME(std::string needle, bool reverse) {
	static std::map<std::string, std::string>	mimeLib = mime_init();
	static std::map<std::string, std::string>	mimeRLib = mime_reverse_init();

	std::map<std::string, std::string>::iterator retval;

	if (!reverse) {
		retval = mimeLib.find(needle);

		if (retval == mimeLib.end())
			return ("application/octet-stream");
	} else {
		retval = mimeRLib.find(needle);
		
		if (retval == mimeRLib.end())
			return ("");
	}

	return (retval->second);
}