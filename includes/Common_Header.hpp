#ifndef COMMON_HEADER_HPP
#define COMMON_HEADER_HPP

#define RECV_BUFFER_SIZE	2048
#define SEND_BUFFER_SIZZ	2046

#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <unordered_map>

#include "./server/Color.hpp"
#include "./ServerExceptions.hpp"

#include "./parse_config/ServerData.hpp"
#include "../srcs/http/request/Request.hpp"
#include "../srcs/http/response/Response.hpp"

#endif
