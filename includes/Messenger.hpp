#ifndef MESSENGER_HPP
#define MESSENGER_HPP

#include "Common_Header.hpp"

struct HttpData
{
	typedef std::unordered_map<std::string, std::string> headers_map;

	int							_client_fd;
	bool						_hasBody;
	std::uint32_t				_body_length;
	std::string 				_http_method;
	std::string					_file_path;
	std::string					_protocol;
	//TODO: возможно надо чистить в ServerCore
	// так же, как _valid_locations
	headers_map					_headers;
	std::string					_body;
	std::string					_transfer_encoding;
};

class Response;

class Messenger
{
	public:
		Messenger(ServerData &_server_data);
		~Messenger();
		void	StartMessaging(const int client_fd, std::string request_text);
		void	ClearValidLocations();

		bool	connectionIsClosed;
	private:
		bool										_toServe; // флаг обозначающий, что SaveRequestData закончен и можно отправлять данные


		/** поля связанные с конфигом */
		std::string						_web_page_name;
		std::string						_root_dir;
		std::multimap<int, std::string>	_valid_locations;

		ServerData 					&_server_data;

		/** поля связанные с данными клиента **/
		HttpData *_client_data;

	//			std::vector<std::string>	_body;

		/** Поля для ответа клиенту*/
		int					_status_code;
		std::string			_status_line;
		std::string 		_file_data;

		Request		_request;
		Response	*_response;

		void				CollectDataForResponse();
		void				SendResponse();
		std::string			ReadFile(std::string file_path, std::string read_method);
		std::string			DefineURLFilePath();
		void				SetValidLocations();
		std::string			GetRootByLocation(std::string &location_path);
		std::string			ConstructFullPath(std::string valid_location);
};

#include "./http/PostMethod.hpp"
#include "./http/GetMethod.hpp"

#endif
