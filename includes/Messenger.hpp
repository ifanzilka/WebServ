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

class ServerKqueue;
class Response;
class Request;

class Messenger
{
	public:
		Messenger(ServerData &server_data, ServerKqueue &server_api);
		~Messenger();
		void	ReadRequest(const int client_fd);
		void	ClearValidLocations();

		bool	isClosed;
	private:
		void			MakeResponse();
		bool			_toServe; // флаг обозначающий, что SaveRequestData закончен и можно отправлять данные

		int			_client_fd;
		Request		_request;
		Response	*_response;
		std::multimap<int, std::string>	_valid_locations;

		ServerKqueue				&_server_api;
		HttpData 					*_client_data;
		ServerData 					&_server_data;
};

#include "./http/PostMethod.hpp"
#include "./http/GetMethod.hpp"

#endif
