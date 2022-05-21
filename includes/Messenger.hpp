#ifndef MESSENGER_HPP
#define MESSENGER_HPP

#include "Common_Header.hpp"

class ServerKqueue;
class Response;
class Request;

class Messenger
{
	public:
		Messenger(ServerData &server_data, ServerKqueue &server_api);
		~Messenger();
		void	MakeResponse();
		void	ReadRequest(const int &client_fd);
		void	ClearValidLocations();
	private:
		std::multimap<int, std::string>	_valid_locations;
		int			_client_fd;

		Request			_request;
		Response		*_response;
		ServerKqueue	&_server_api;
};

#endif
