#include "File.hpp"

File::File() {
	type = "none";
}

File::File(const std::string filename) {
	std::ifstream file(filename.c_str());

	for (std::string line; std::getline(file, line);) {
		contents << line << '\n';
		size += line.size();
	}
	
	file.close();
}

File::File(const char *filename) {
	std::ifstream file(filename);

	for (std::string line; std::getline(file, line);) {
		contents << line << '\n';
		size += line.size();
	}
	
	file.close();
}

File::~File() {

}

void	File::open(const std::string filename) {
	std::ifstream file(filename.c_str());

	for (std::string line; std::getline(file, line);) {
		contents << line << '\n';
		size += line.size();
	}
	
	file.close();
}

void	File::open(const char *filename) {
	std::ifstream file(filename);

	for (std::string line; std::getline(file, line);) {
		contents << line << '\n';
		size += line.size();
	}
	
	file.close();
}

void	File::write(const std::string str) {
	contents << str << std::flush;
	size += str.size();
}

void	File::write(const char *str) {
	contents << str << std::flush;
	size += strlen(str);
}

File	&operator<<(File &model, const std::string str) {
	model.getStream() << str << std::flush;
	model.getSize() += str.size();
	return (model);
}

File	&operator<<(File &model, const char *str) {
	model.getStream() << str << std::flush;
	model.getSize() += strlen(str);
	return (model);
}

std::ostream	&operator<<(std::ostream &stream, File &model) {
	std::cout << model.getStream() << std::endl;
	stream << model.getStream();
	return (stream);
}