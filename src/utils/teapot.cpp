#include "webserv.hpp"

std::ifstream	teapotGenerator() {
	std::stringstream retval;

	retval << "<!DOCTYPE html>\n";
	retval << "<h1> 🫖 418 - I'm a Teapot!!!!!! 🫖 </h1>\n";
	retval << "<p>🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖</p>\n";
	retval << "<p>🫖 The requested teapot has been returned succesfully!!!!! 🫖</p>\n";
	retval << "<strong>🫖 ENJOY YOUR TEAPOT!!!! 🫖</strong>";
}