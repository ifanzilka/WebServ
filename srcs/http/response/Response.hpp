//
// Created by Josephine Beregond on 5/2/22.
//

#ifndef RESPONSE_HPP
#define RESPONSE_HPP


class Response
{
	public:

	private:
		std::uint32_t	_status_code;
		std::string		_status_line;

		std::string		_headers;
		std::string		_http_method;

		std::string		_body;
		std::uint64_t	_body_len;
};


#endif
