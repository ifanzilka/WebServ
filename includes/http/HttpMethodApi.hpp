#ifndef HTTPMETHODAPI_HPP
#define HTTPMETHODAPI_HPP

//#include "../Messenger.hpp"

//TODO: сделать класс полностью абстрактным
struct HttpData;

class HttpMethodApi
{
	public:
		HttpMethodApi();
		virtual			~HttpMethodApi();
		std::string 	GetContentType() const;
		int				GetContentLength() const;
		virtual	int SendHttpResponse(int client_fd, std::vector<char> &file_buffer,
			std::string _http_code, HttpData *_client_data) = 0;
	protected:

	private:
		std::string	_content_type;
		int			_content_length;
};


#endif
