#include "File.hpp"

File::File() {
	type = "none";
	_size = 0;
}

File::File(const std::string filename, 
	fileIterator start, fileIterator end) {
	name = filename;
	write(start, end);
}

File::File(const std::string filename) {
	std::ifstream	file;

	file.open(filename.c_str(), std::ios::binary);

    file.seekg(0, std::ios::end);
    _size = file.tellg();
    file.seekg(0, std::ios::beg);

	body = std::vector<char>(_size);
	file.read((char*) &body[0], _size);
}

File::File(const char *filename) {
	std::ifstream	file;

	file.open(filename, std::ios::binary);

    file.seekg(0, std::ios::end);
    _size = file.tellg();
    file.seekg(0, std::ios::beg);

	body = std::vector<char>(_size);
	file.read((char*) &body[0], _size);
}

File::~File() {

}

void	File::open(const std::string filename) {
	std::ifstream	file;

	file.open(filename.c_str(), std::ios::binary);

    file.seekg(0, std::ios::end);
    _size = file.tellg();
    file.seekg(0, std::ios::beg);

	body = std::vector<char>(_size);
	file.read((char*) &body[0], _size);
}

void	File::open(const char *filename) {
	std::ifstream	file;

	file.open(filename, std::ios::binary);

    file.seekg(0, std::ios::end);
    _size = file.tellg();
    file.seekg(0, std::ios::beg);

	body = std::vector<char>(_size);
	file.read((char*) &body[0], _size);
}

void	File::write(fileIterator &start, fileIterator &end) {
	_size = std::distance(start, end);
	
	for (; start != end; ++start)
		body.push_back(*start);
}

void	File::write(const char *str) {
	for (size_t i = 0; str[i]; i++)
		body.push_back(str[i]);

	_size += cstrlen(str);
}

void	File::toDisk(std::string filename) {
	int fd = ::open(filename.c_str(), O_CREAT | O_WRONLY);
	
	for (fileIterator it = body.begin(); it != body.end(); ++it) {
		::write(fd, &*it, 1);
	}
	close(fd);
}