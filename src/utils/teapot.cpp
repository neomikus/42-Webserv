#include "webserv.hpp"
#include "File.hpp"

void	teapotGenerator(File &responseBody) {
	responseBody << "<!DOCTYPE html>" << "\n";
	responseBody << "<h1> 🫖 418 - I'm a Teapot!!!!!! 🫖 </h1>" << "\n";
	responseBody << "<p>🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖</p>" << "\n";
	responseBody << "<p>🫖 The requested teapot has been returned succesfully!!!!! 🫖</p>" << "\n";
	responseBody << "<strong>🫖 ENJOY YOUR TEAPOT!!!! 🫖</strong>" << "\n";
}