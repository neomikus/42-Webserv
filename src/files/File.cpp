#include "File.hpp"

File::File() {
	type = "none";
	_size = 0;
}

File::File(const std::string filename) {
	std::ifstream file(filename.c_str(), std::ios::binary);

	for (std::string line; std::getline(file, line);) {
		contents.write((line + '\n').c_str(), cstrlen((line + '\n').c_str()));
		_size += line.size();
	}
	
	file.close();
}

File::File(const char *filename) {
	std::ifstream file(filename, std::ios::binary);

	for (std::string line; std::getline(file, line);) {
		contents.write((line + '\n').c_str(), cstrlen((line + '\n').c_str()));
		_size += line.size();
	}
	
	file.close();
}

File::~File() {

}

void	File::open(const std::string filename) {
	std::ifstream file(filename.c_str(), std::ios::binary);

	for (std::string line; std::getline(file, line);) {
		contents.write((line + '\n').c_str(), cstrlen((line + '\n').c_str()));
		_size += line.size();
	}
	
	file.close();
}

void	File::open(const char *filename) {
	std::ifstream file(filename, std::ios::binary);

	for (std::string line; std::getline(file, line);) {
		contents.write((line + '\n').c_str(), cstrlen((line + '\n').c_str()));
		_size += line.size();
	}
	
	file.close();
}

void	File::write(const std::string str) {
	contents.write(str.c_str(), cstrlen(str.c_str()));
	_size += str.size();
}

void	File::write(const char *str) {
	contents.write(str, cstrlen(str));
	_size += cstrlen(str);
}

std::string	File::read() {
	char *str = new char[_size];
	contents.read(str, _size);
	std::string	retval = str;
	delete[] str;
	return (retval);
}

void	File::read(char *str, size_t size) {
	contents.read(str, size);
}


File	&operator<<(File &model, const std::string &str) {
	model.getStream().write(str.c_str(), cstrlen(str.c_str()));
	model.getSize() += str.size();
	return (model);
}

File	&operator<<(File &model, const char *str) {
	model.getStream().write(str, cstrlen(str));
	model.getSize() += cstrlen(str);
	return (model);
}

std::ostream	&operator<<(std::ostream &stream, File &model) {
	char *str = new char[model.getSize()];
	model.getStream().read(str, model.getSize());
	stream << str;
	delete[] str;
	return (stream);
}