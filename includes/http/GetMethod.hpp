#ifndef GETMETHOD_HPP
#define GETMETHOD_HPP

#include "HttpMethodApi.hpp"

#include "../Messenger.hpp"

class GetMethod : public HttpMethodApi
{
	public:
		GetMethod();
		virtual		~GetMethod();
		virtual		int SendHttpResponse(int client_fd, std::string &file_buffer,
			std::string _http_code, HttpData *_client_data);
	private:

};

#endif
