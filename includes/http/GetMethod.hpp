#ifndef GETMETHOD_HPP
#define GETMETHOD_HPP

#include "HttpMethodApi.hpp"

class GetMethod : public HttpMethodApi
{
	public:
		GetMethod();
		virtual		~GetMethod();
		virtual		int SendHttpResponse(int client_fd, std::vector<char> &file_buffer,
			std::string _http_code);
	private:

};

#endif
