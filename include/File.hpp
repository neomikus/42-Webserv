#ifndef FILE_HPP
#define FILE_HPP

#include "webserv.hpp"

class File
{
	private:
		std::stringstream	contents;
		std::string			type;
		std::string			name;
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

		std::stringstream	&getStream() {return (contents);}
		long long			&getSize() {return (size);}
		std::string			getType() const {return(name);}
		std::string			getName() const {return(name);}

		void				setType(std::string newType) {type = newType;}
		void				setName(std::string newName) {name = newName;}

};

File			&operator<<(File &model, const std::string &str);
File			&operator<<(File &model, const char *str);
std::ostream	&operator<<(std::ostream &stream, File &model);	

#endif