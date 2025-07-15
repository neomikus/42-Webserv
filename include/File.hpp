#ifndef FILE_HPP
#define FILE_HPP

#include "webserv.hpp"

class File
{
	private:
		std::stringstream	contents;
		std::string			type;
		long long			size;
	public:
		File();
		File(const std::string filename);
		File(const char *filename);
		~File();

		void	open(const std::string filename);
		void	open(const char *filename);
		void	write(const std::string str);
		void	write(const char *str);

		std::stringstream	&getStream() {return (contents);};
		long long			&getSize() {return (size);};
};

File			&operator<<(File &model, const std::string str);
File			&operator<<(File &model, const char *str);
std::ostream	&operator<<(std::ostream &stream, File &model);	

#endif