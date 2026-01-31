#include "webserv.hpp"

std::string	sizeStr(off_t size) {
	const char *array[4] = {"", "kb", "mb", "gb"};
	std::stringstream retval;
	int	mag = 0;
	float newSize = size;

	while (newSize > 1024 && mag < 3) {
		newSize /= 1024;
		mag++;
	}

	if ((int)newSize == newSize)
		retval << (int)newSize << array[mag];
	else
		retval << std::fixed << std::setprecision(1) << newSize << array[mag];
	return (retval.str());
}

std::string	getFileType(std::string file, struct stat fileBuffer) {

	if (S_ISDIR(fileBuffer.st_mode))
		return ("directory");
	// If it's a file, identify which type
	
	if (file.find_last_of(".") == file.npos)
		return ("file");
	std::string mime = getMIME(file.substr(file.find_last_of(".")), false);
	
	if (mime == "text/html")
		return ("webpage");

	if (mime == "application/pdf")
		return ("text");

	if (trimLastWord(mime, '/') == "image" || trimLastWord(mime, '/') == "audio" || trimLastWord(mime, '/') == "video" || 
		trimLastWord(mime, '/') == "text" || (trimLastWord(mime, '/') == "application" && mime != "application/octet-stream"))
		return (trimLastWord(mime, '/'));
	
	return("file");
}

std::string	getFileElement(std::string current, std::string path) {
	std::stringstream retval;
	struct stat fileBuffer;
	std::string	type;

	stat((path + current).c_str(), &fileBuffer);

	type = getFileType(current, fileBuffer);
	// Name cell
	retval << "<tr><td class='" << type << "'>" << std::endl;
	retval << "<a href='" << current << "'>" << current << "</a></td>" << std::endl;

	// Last modified cell
	retval << "<td>" << ctime(&fileBuffer.st_mtime) << "</td>" << std::endl;

	// Size cell
	if (type == "directory")
		retval << "<td>-</td>" << std::endl;
	else
		retval << "<td>" << sizeStr(fileBuffer.st_size) << "</td>" << std::endl;

	return (retval.str());
}

bool	compareStr(std::string a, std::string b) {
	return (a < b);
}

File	generateAutoIndex(std::string resource, std::string directory) {
	File	retval;

	DIR		*dirPTR;
	if (!directory.empty())
		dirPTR = opendir(directory.c_str());
	else
		dirPTR = opendir(".");

	if (directory[directory.size() - 1] != '/')
		directory += "/";

	std::vector<std::string>	filenames;

	std::stringstream contents;

	contents << "<!DOCTYPE html>" << std::endl;
	contents << "<head> <meta charset='UTF-8'> <meta name='viewport' content='width=device-width, initial-scale=1.0'> <title>Autoindex</title> <style>" << std::endl;
	contents << "body{margin: 30px; font-size: 18px;} header{border-bottom: 0.5px solid black; padding: 0; margin: 0; margin-bottom: 20px;} h1{margin: 10px; margin-left: 10px;}" << std::endl; 
	contents << "a{color:inherit; text-decoration: none;} a:vistied{color:inherit;} a:hover{text-decoration: underline;}" << std::endl;
	contents << "table{border-collapse: collapse; display:grid; grid-template-rows: repeat(3, 1fr); width: 100%; max-height: 50px;} .header{font-size: 1.2em; font-weight: bold; } td:first-child{width: 20%; text-align: left;} td{margin: auto; text-align: center;} tr{margin-bottom: 2px; border-bottom: 1px solid gray;}" << std::endl;
	contents << ".directory{color: green;} .directory::before{font-family: 'Font Awesome 5 Free'; font-weight: 900; content: '\\f07c  '; margin-left: -3px; margin-right: 0}" << std::endl;
	contents << ".file{color: gray;} .file::before{font-family: 'Font Awesome 5 Free'; font-weight: 900; content: '\\f15b  ';}" << std::endl;
	contents << ".image{color: blue;} .image::before{font-family: 'Font Awesome 5 Free'; font-weight: 900; content: '\\f1c5  ';}" << std::endl;
	contents << ".audio{color: #FF647F;} .audio::before{font-family: 'Font Awesome 5 Free'; font-weight: 900; content: '\\f1c7  ';}" << std::endl;
	contents << ".video{color: black;} .video::before{font-family: 'Font Awesome 5 Free'; font-weight: 900; content: '\\f1c8  ';}" << std::endl;
	contents << ".webpage{color: #E0BC00;} .webpage::before{font-family: 'Font Awesome 5 Free'; font-weight: 900; content: '\\f1c9  ';}" << std::endl;
	contents << ".text{color: crimson;} .text::before{font-family: 'Font Awesome 5 Free'; font-weight: 900; content: '\\f15c  ';}" << std::endl;
	contents << ".application{color: crimson;} .application::before{font-family: 'Font Awesome 5 Free'; font-weight: 900; content: '\\e697  ';}" << std::endl;
	contents << "</style></head>" << std::endl;	

	contents << "<body><script src='https://kit.fontawesome.com/b096cabf1e.js' crossorigin='anonymous'></script>" << "<header><h1>Index of " << (resource.empty() ? "/" : resource.c_str()) << "</h1></header>" << std::endl << "<table>" << std::endl;
	contents << "<tr class='header'><td>File</td><td>Last modified</td><td>Size</td></tr>" << std::endl;

	if (!resource.empty()) {
		contents << getFileElement("..", directory) << std::endl;
	}

	for (dirent *current = readdir(dirPTR); current; current = readdir(dirPTR))
		filenames.push_back(current->d_name);

	std::sort(filenames.begin(), filenames.end(), compareStr);

	for (std::vector<std::string>::iterator it = filenames.begin(); it != filenames.end(); ++it) {
		if ((*it)[0] == '.')
			continue;

		struct stat dirBuffer;
		stat((directory + *it).c_str(), &dirBuffer);

		if (S_ISDIR(dirBuffer.st_mode))
			contents << getFileElement(*it, directory) << std::endl;
	}

	for (std::vector<std::string>::iterator it = filenames.begin(); it != filenames.end(); ++it) {
		if ((*it)[0] == '.')
			continue;

		struct stat dirBuffer;
		stat((directory + *it).c_str(), &dirBuffer);

		if (!S_ISDIR(dirBuffer.st_mode))
			contents << getFileElement(*it, directory) << std::endl;
	}


	contents << "</table></body>" << std::endl;

	retval.write(contents.str().c_str());
	retval.setType("text/html");
	closedir(dirPTR);
	return (retval);
}