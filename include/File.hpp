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
		void	write(fileIterator start, fileIterator end);
		void	write(const char *str);
		
		void	toDisk(std::string filename);

		std::vector<char>	&getBody();
		long long			&getSize();
		std::string			getType() const;
		std::string			&getName();
		std::string			getName() const;

		void				setType(std::string newType);
		void				setName(std::string newName);
};

#endif