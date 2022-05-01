#ifndef MESSENGER_HPP
#define MESSENGER_HPP

#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>

#include "./server/Color.hpp"
#include "./ServerExceptions.hpp"

#include "./parse_config/ServerData.hpp"

//#include "Include_Http_Library.hpp"

#include "../srcs/http/request/Request.hpp"

#include <unordered_map>

struct HttpData
{
	int							_client_fd;
	std::string 				_http_method;
	std::string					_file_path;
	std::string					_protocol;
	//TODO: возможно надо чистить в ServerCore
	// так же, как _valid_locations
	std::unordered_map<std::string, std::string>	_headers;
};

class Messenger
{
	public:
		Messenger(ServerData &_server_data);
		~Messenger();
		void	StartMessaging(const int client_fd, std::string request_text);
		void	ClearValidLocations();
	private:
		/** поля связанные с конфигом */
		std::string						_web_page_name;
		std::string						_root_dir;
		std::multimap<int, std::string>	_valid_locations;

		ServerData 					&_server_data;

		/** поля связанные с данными клиента **/
		HttpData *_client_data;

	//			std::vector<std::string>	_body;

		/** Поля для ответа клиенту*/
		std::string 		_status_code;
		std::vector<char> _file_data;

		void				CollectDataForResponse();
		void				SendResponse();
		std::vector<char>	ReadFile(std::string file_path, std::string read_method);
		std::string			DefineURLFilePath();
		void				SetValidLocations();
		std::string			GetRootByLocation(std::string &location_path);
		std::string			ConstructFullPath(std::string valid_location);
};

#include "./http/PostMethod.hpp"
#include "./http/GetMethod.hpp"

#endif
