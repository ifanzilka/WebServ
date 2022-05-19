#ifndef COMMON_HEADER_HPP
#define COMMON_HEADER_HPP

#define RECV_BUFFER_SIZE	4096
#define SEND_BUFFER_SIZZ	2046

#define CR				"\r"
#define LF				"\n"
#define CRLF			"\r\n"

//#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <unordered_map>

#include <sys/socket.h>

typedef enum	fileType
{
	DDIR  = 0,
	FFILE  = 1,
	NONEXIST = -1
}	t_fileType;

typedef struct s_fileInfo
{
	long long int	fLength;
	t_fileType		fType;
	std::string		fExtension;
	int				fStatus;
}		t_fileInfo;

#include "./server/Color.hpp"
#include "./ServerExceptions.hpp"

#include "./parse_config/ServerData.hpp"
#include "../srcs/http/request/Request.hpp"
#include "../srcs/http/response/Response.hpp"

#endif
