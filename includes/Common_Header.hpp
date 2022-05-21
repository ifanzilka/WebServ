#ifndef COMMON_HEADER_HPP
#define COMMON_HEADER_HPP

#define RECV_BUFFER_SIZE	4096
//#define SEND_BUFFER_SIZE	4096
#define SEND_BUFFER_SIZE	2048

#define CR				"\r"
#define LF				"\n"
#define CRLF			"\r\n"

/* kqueue */
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <signal.h> // kill

//#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <unordered_map>

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

#include "Include_Library.hpp"

#include "./parse_config/ServerData.hpp"

#include "../srcs/http/request/Request.hpp"
#include "../srcs/http/response/Response.hpp"

#endif
