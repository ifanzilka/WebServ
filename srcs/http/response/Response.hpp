//
// Created by Josephine Beregond on 5/2/22.
//

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Common_Header.hpp"
#include "server/ServerKqueue.hpp"
#include "CGI.hpp"

class CGI;
class Request;
class ServerKqueue;

class Response
{
	public:
		Response(Request &request);
		~Response();
		void	SendResponse(int client_fd);
		bool	isSent() { return (!_left_bytes); }

	private:

	/** Initialization part (constructor) */

		uint16_t 	CGI_Amount(const std::multimap<std::string, std::string> &CGI, const std::string &file_path);
		void		CollectDataForResponse(const t_fileInfo &file);
		void		FillFileInfo(const std::string &file_path, t_fileInfo *const file_t, std::ifstream &FILE);
		std::string	MakeFilePath(Request &request, uint32_t &status_code);
		void 		CollectStartData(void);

	/** SendResponse part */

		void				CloseCGI(void);
		void				SendBody(const int &client_fd, int &read_size, int &read_bytes);
		char				*MakeBody(int &readSize);
		const std::string	&MakeHeadersBlock();
		const std::string	&MakeStatusLine();

	/** Utils */

		const std::string	&GetErrorPage();
		void				SetErrorPages();


/**
 * ====================================
 * ==            Variables           ==
 * ====================================
 */

		/**  CGI variables */
		pid_t								_pid;
		CGI									*_cgi_ptr;
		const int							*_cgi_fds;

		/** Class' variables */
		std::map<std::string, std::string>	_req_headers;
		std::map<int, std::string>			_error_pages;
		const LocationData					*_req_location;
		Request								&_request;
		std::ifstream						_FILE;
		std::uint64_t						_left_bytes; // для проверки вся ли информация записалась
		std::uint64_t						_body_size;
		std::uint32_t						_status_code;
		std::string							_headers;
		std::string							_status_line;
		std::string							_response;
		std::string							_url;
		std::string							_method;
		std::string							_content_type;
		char*						 		_body;
		bool								_inProc; // false в конце конструктора Response();
		bool								_autoindex;
};


#endif
