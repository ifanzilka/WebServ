#ifndef MESSENGER_HPP
#define MESSENGER_HPP

#include "RequestParser.hpp"

#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>

#include "./parse_config/ServerData.hpp"
#include "./http/GetMethod.hpp"

namespace ft
{
	class Messenger
	{
		public:
			Messenger(ServerData &_server_data);
			~Messenger();
			void	SetClientFd(const int client_fd);
			int		SetRequest(const int client_fd, std::string request);
		private:
			ServerData 					&_server_data;
			int							_client_fd;
			std::string 				_http_method;
			std::string					_file_path;
			std::vector<std::string>	_headers;
	//			std::vector<std::string>	_body;

			void				SendResponse();
			std::vector<char>	ReadFile(std::string file_path, std::string read_method);
	};
}

#endif
