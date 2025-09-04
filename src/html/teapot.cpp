#include "webserv.hpp"
#include "File.hpp"

void	teapotGenerator(File &responseBody) {
	responseBody.write("<!DOCTYPE html>\n");
	responseBody.write("<head><meta charset='UTF-8' /></head>\n");
	responseBody.write("<h1> 🫖 418 - I'm a Teapot!!!!!! 🫖 </h1>\n");
	responseBody.write("<p>🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖🫖</p>\n");
	responseBody.write("<p>🫖 The requested teapot has been returned succesfully!!!!! 🫖</p>\n");
	responseBody.write("<strong>🫖 ENJOY YOUR TEAPOT!!!! 🫖</strong>\n");
}