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
	int fd = ::open(filename.c_str(), O_RDONLY);
	char buffer[BUFFER_SIZE + 1];
	name = filename;

	for (int rd = ::read(fd, buffer, BUFFER_SIZE); rd > 0; rd = ::read(fd, buffer, BUFFER_SIZE)) {
		buffer[rd] = '\0';
		write(buffer);
		_size += rd;
	}

	close(fd);
}

File::File(const char *filename) {
	int fd = ::open(filename, O_RDONLY);
	char buffer[BUFFER_SIZE + 1];
	name = filename;

	for (int rd = ::read(fd, buffer, BUFFER_SIZE); rd > 0; rd = ::read(fd, buffer, BUFFER_SIZE)) {
		buffer[rd] = '\0';
		write(buffer);
		_size += rd;
	}

	close(fd);
}

File::~File() {

}

void	File::open(const std::string filename) {
	int fd = ::open(filename.c_str(), O_RDONLY);
	char buffer[BUFFER_SIZE + 1];
	name = filename;

	for (int rd = ::read(fd, buffer, BUFFER_SIZE); rd > 0; rd = ::read(fd, buffer, BUFFER_SIZE)) {
		buffer[rd] = '\0';
		write(buffer);
		_size += rd;
	}

	close(fd);
}

void	File::open(const char *filename) {
	int fd = ::open(filename, O_RDONLY);
	char buffer[BUFFER_SIZE + 1];
	name = filename;

	for (int rd = ::read(fd, buffer, BUFFER_SIZE); rd > 0; rd = ::read(fd, buffer, BUFFER_SIZE)) {
		buffer[rd] = '\0';
		write(buffer);
		_size += rd;
	}

	close(fd);
}

void	File::write(fileIterator &start, fileIterator &end) {
	_size = std::distance(start, end);
	
	for (; start != end; ++start)
		body.push_back(*start);
}

void	File::write(const char *str) {
	for (size_t i = 0; str[i]; i++) {
		body.push_back(str[i]);
	}
	
	_size += cstrlen(str);
}

void	File::toDisk(std::string filename) {
	int fd = ::open(filename.c_str(), O_CREAT | O_WRONLY);
	
	for (long long i = 0; i < _size; i++)
		::write(fd, &body[i], 1);

	close(fd);
}
