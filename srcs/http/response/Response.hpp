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

	private:
		std::string	getErrorPage();
		void	setErrorPages();

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
