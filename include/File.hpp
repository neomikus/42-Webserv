#ifndef FILE_HPP
#define FILE_HPP

#include "webserv.hpp"

class File
{
	private:		
		std::vector<char>	body;
		std::string			type;
		std::string			name;
		long long			_size;
	
	public:
		typedef	std::vector<char>::iterator fileIterator;
		File();
		File(const std::string filename);
		File(const char *filename);
		File(const std::string filename, 
			fileIterator start, fileIterator end);
		~File();

		void	open(const std::string filename);
		void	open(const char *filename);
		void	write(fileIterator &start, fileIterator &end);
		void	write(const char *str);
		
		void	toDisk(std::string filename);

		std::vector<char>	&getBody() {return (body);}
		long long			&getSize() {return (_size);}
		std::string			getType() const {return(type);}
		std::string			&getName() {return(name);}
		std::string			getName() const {return(name);}

		void				setType(std::string newType) {type = newType;}
		void				setName(std::string newName) {name = newName;}
};

#endif