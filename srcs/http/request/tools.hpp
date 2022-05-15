#ifndef PRACTISING_SERVER_TOOLS_HPP
#define PRACTISING_SERVER_TOOLS_HPP

#include "Common_Header.hpp"

#include <sys/stat.h> // 'stat' struct

/** FOR IsDirOrFile */
#define NOT_FOUND		0
#define FILE_MODE		1
#define DIR_MODE		2
#define UNKNOWN_MODE	3

class Request;

/**
* RESPONSE_DATA PART
*/
std::uint8_t	isDirOrFile(std::string &path);
std::string		putDelete(Request &request, uint32_t &statusCode);

/**
* SAVE_REQUEST_DATA PART
*/
std::size_t	skipWhiteSpaces(std::string const &str, std::size_t start);
bool	isCharWhiteSpace(unsigned char c);


#endif
