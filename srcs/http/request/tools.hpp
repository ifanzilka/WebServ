#ifndef PRACTISING_SERVER_TOOLS_HPP
#define PRACTISING_SERVER_TOOLS_HPP

#include "Common_Header.hpp"

#include <sys/stat.h> // 'stat' struct
#include <dirent.h>

/** FOR GetTypeOfData */
#define NOT_FOUND		0
#define FILE_MODE		1
#define DIR_MODE		2
#define UNKNOWN_MODE	3

class Request;

/**
* CGI PART
*/

off_t getFdLen(int fd);
int  checkCgi(const std::multimap<std::string, std::string> &CGI, std::string fPath);

/**
* RESPONSE_DATA PART
*/

static std::string	buildPathToFile(std::string const &fullPath, const LocationData *location, std::string fileName);
char	*filesListing(std::string const &path, uint64_t &bodySize, uint32_t &statusCode, const LocationData *location);
std::map<int, std::string>	&error_map();
std::string					ft_itoa(int x);
char						*gen_def_page(uint32_t &statusCode, uint64_t &bodySize, const char *path, const LocationData *location);
std::string					strUpper(const std::string &str);
std::string					getExtension(std::string fPath);
bool						urlInfo(std::string fPath, t_fileInfo *fStruct, std::ifstream &FILE);
std::uint8_t				GetTypeOfData(const std::string &path);
std::string					putDelete(Request &request, uint32_t &statusCode);

/**
* SAVE_REQUEST_DATA PART
*/
bool	isCharWhiteSpace(unsigned char c);


#endif
