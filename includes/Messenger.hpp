#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include <unistd.h>
#include <iostream>
#include "Color.hpp"
#include <string>

namespace ft
{
	class Messenger
	{
		public:
			Messenger();
//			Messenger(const int client_fd);
			~Messenger();
			void	SetClientFd(const int client_fd);
			int		SetRequest(const int client_fd, std::string request);
		private:
			int							_client_fd;
//			std::string 				_http_method;
//			std::string					_file_path;
//			std::vector<std::string>	_headers;
//			std::vector<std::string>	_body;

			void	SendResponse(const int client_fd);
	};
}

#endif
