#ifndef HTTPMETHODAPI_HPP
#define HTTPMETHODAPI_HPP

//TODO: сделать класс полностью абстрактным
class HttpMethodApi
{
	public:
		HttpMethodApi();
		virtual			~HttpMethodApi();
		std::string 	GetContentType() const;
		int				GetContentLength() const;
		virtual		int SendHttpResponse(int client_fd, std::vector<char>	file_buffer) = 0;
	protected:

	private:
		std::string	_content_type;
		int			_content_length;
};


#endif
