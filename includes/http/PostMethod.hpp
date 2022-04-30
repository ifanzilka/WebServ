#ifndef POSTMETHOD_HPP
#define POSTMETHOD_HPP

#include "HttpMethodApi.hpp"

class PostMethod : public HttpMethodApi
{
	public:
		PostMethod();
		virtual		~PostMethod();
		virtual		int SendHttpResponse(int client_fd, std::vector<char> &file_buffer,
			std::string _http_code, HttpData *_client_data);
	private:

};

#endif