#ifndef PRACTISING_SERVER_SERVER_UTILS_HPP
#define PRACTISING_SERVER_SERVER_UTILS_HPP

#include "Common_Header.hpp"

#include <sys/stat.h> // 'stat' struct
#include <dirent.h>

/** FOR GetDataType */
#define NOT_FOUND		0
#define FILE_MODE		1
#define DIR_MODE		2
#define UNKNOWN_MODE	3

class Request;

/** CGI PART */
off_t GetFdLen(const int &fd);

/** RESPONSE_DATA PART */
static std::string		MakeFullPath(std::string const &file_path,
	const LocationData *location, const std::string &file_name);

char	*Autoindex(std::string const &path, uint64_t &body_size,
	uint32_t &status_code, const LocationData *location);

char	*MakePage(uint32_t &status_code, uint64_t &bodySize,
	const char *path, const LocationData *location);

std::map<int, std::string>	&GetErrorTypes();
std::string					UpperStr(const std::string &str);
std::string					GetFileExtension(const std::string &file_path);
std::uint8_t				GetDataType(const std::string &data_path);

/** SAVE_REQUEST_DATA PART */
bool	IsChar(unsigned char c);

#endif
