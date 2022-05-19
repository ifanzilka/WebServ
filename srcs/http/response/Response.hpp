//
// Created by Josephine Beregond on 5/2/22.
//

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Common_Header.hpp"

class Request;

class Response
{
	public:
		Response(Request &request);
		~Response();
		void SendResponse(int client_fd);

	private:
		char		*makeBody(int &readSize);
		std::string makeHeaders();
		std::string	makeStatusLine();
		std::string	getErrorPage();
		void		setErrorPages();

		char*						 		_body;

		std::uint64_t						_leftBytes; // для проверки вся ли информация записалась
		std::string							_headers;
		std::string							_statusLine;
		std::string							_response;
		bool								_inProc; // false в конце конструктора Response();

		std::ifstream						_FILE;
		bool								_autoindex;
		const LocationData					*_reqLocation;
		std::string							_url;
		std::string							_method;
		std::map <std::string, std::string>	_reqHeaders;
		std::uint64_t						_bodySize;
		uint32_t							_statusCode;
		std::string							_contentType;
		std::map<int, std::string>			_errorPages;
};


#endif
