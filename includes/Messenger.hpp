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
			void	ClearValidLocations();
	private:
			/** поля связанные с конфигом */
			std::string						_web_page_name;
			std::string						_root_dir;
			std::multimap<int, std::string>	_valid_locations;

			ServerData 					&_server_data;

			/** поля связанные с данными клиента **/
			int							_client_fd;
			std::string 				_http_method;
			std::string					_file_path;
			//TODO: возможно надо чистить в ServerCore
			// так же, как _valid_locations
			std::vector<std::string>	_headers;
		//			std::vector<std::string>	_body;

			void				SendResponse();
			std::vector<char>	ReadFile(std::string file_path, std::string read_method);
			std::string			DefineURLFilePath();
			void				SetValidLocations();
			void				SetDataViaConfig();
			std::string			GetRootByLocation(std::string &location_path);
			std::string			ConstructFullPath(std::string valid_location);
	};
}

#endif
