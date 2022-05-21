//
// Created by Josephine Beregond on 4/30/22.
//

#ifndef REQUEST_H
#define REQUEST_H

#include "./Common_Header.hpp"
#include "server/ServerKqueue.hpp"
#include "tools.hpp"

# define START_LINE		0
# define HEADER_LINE	1
# define BODY_LINE		2
# define END_STATE		3
# define HTTP_PROTOCOL	"HTTP/1.1"

class ServerKqueue;

class Request
{
	public:
		Request(std::multimap<std::string, LocationData> &locations, ServerKqueue &server_api);
		~Request();

		/**
		* CGI PART
		*/
		std::string	GetURIParameters(void); // CGI()
		void		getUrlEncodedBody(std::map<std::string, std::string> &queryBody); // CGI()

		/**
		* RESPONSE_DATA PART
		*/
		const std::string							&GetBody(void); // putDelete()
		const LocationData							*GetLocation() const; // Response(), putDelete(), CGI()
		std::string									getUrl(std::uint32_t &status); // CGI() && PutDelete
		const std::string							&GetMethod(void) const;  // Response()
		const std::map<std::string, std::string>	&GetHeaders(void) const; // Response()
		const std::uint32_t							&GetStatusCode(void) const; // Response()

		/**
		 * SAVE_REQUEST_DATA PART
		 * functions for Messenger::ReadRequest()
		 */
		void	SetClientFd(const std::uint8_t &fd);
		void	PrintAllRequestData();
		void	SetStatusCode(const std::uint32_t &status_code);
		char	*GetBuffer(void) const;
		void	ReadRequestData(size_t data_size);

	private:
		/**
		* SAVE_REQUEST_DATA PART
		*/
		std::string	validateUrl(std::string &fullPath, std::uint32_t &status, std::uint8_t mode);

		void	saveSimpleBody(std::string &data);
		void	parseChunkedBody(std::string &data);
		void	parseChunkSize(std::string &data);
		void	saveChunkedBody(std::string &data);
		void	saveHeaderLine(std::string headerLine);

		void	ParsePercentData(std::string &uri_ref);
		void	ParseURIData(void);
		const 	LocationData	*GetValidLocation(void);
		void	SaveProtocol(std::string &fst_line, std::size_t &space_ind);
		void	SaveURI(std::string &fst_line, std::size_t &space_ind);
		void	SaveMethod(std::string &fst_line, std::size_t &space_ind);
		void	ReadStartLine(std::string fst_line);
		void	ReadFirstBlock(std::string &req_data); /** чтение первой строки и заголовков */

		void	resetRequest(void);

		std::uint8_t									_client_fd;
		ServerKqueue									&_server_api;
		std::multimap<std::string, LocationData> const	&_allLocations;
		const LocationData								*_location;
		char											*_buffer;

		/*
		 * SAVE_REQUEST_DATA PART
		 */
		std::string							_body;
		std::uint32_t						_chunkSize; // размер пересылаемых данных
		bool								_hasChunk; // saveChunkedBody()
		std::uint32_t						_bodySize;
		std::string							_transferEncoding;
		std::map<std::string, std::string>	_headers; // ключ:значение
		std::string							_uri_parameters; // строка запроса после '?'
		std::uint32_t						_maxBodySize;
		std::uint8_t						_parseState;
		std::string							_tmpBuffer;
		std::string							_method;
		std::string							_protocol;
		std::string							_uri;
		std::uint32_t						_status_code;
};

#include "Messenger.hpp"

#endif
