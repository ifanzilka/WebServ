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

		void	ReadRequestData(const size_t &data_size);
		void	PrintAllRequestData(void);

		/**
		* CGI PART
		*/
		std::string	GetURIParameters(void); // CGI()
		void		GetUrlEncodedBody(std::map<std::string, std::string> &query_body); // CGI()

		/**
		* RESPONSE_DATA PART
		*/
		const std::string							&GetBody(void); // putDelete()
		const LocationData							*GetLocation() const; // Response(), putDelete(), CGI()
		const std::string							GetUrl(std::uint32_t &status_code); // CGI() && PutDelete
		const std::string							&GetMethod(void) const;  // Response()
		const std::map<std::string, std::string>	&GetHeaders(void) const; // Response()
		const std::uint32_t							&GetStatusCode(void) const; // Response()

		/**
		 * SAVE_REQUEST_DATA PART
		 * functions for Messenger::ReadRequest()
		 */
		void	SetClientFd(const std::uint8_t &fd);
		void	SetStatusCode(const std::uint32_t &status_code);
		char	*GetBuffer(void) const;
	private:

		void	ClearRequestData(void);
		/** вспомогательный метод для GetUrl() */
		std::string	ValidateUrl(std::string &full_path, std::uint32_t &status_code, std::uint8_t mode);

/**
 * ====================================
 * ==          Saving body           ==
 * ====================================
 */

		void	SaveCommonBody(std::string &req_data);
		void	ParseChunkedBody(std::string &req_data);
		void	TryToFindChunk(std::string &req_data);
		void	SaveChunkedBody(std::string &req_data);

/**
 * ====================================
 * == Saving first line and headers  ==
 * ====================================
 */

		/** SAVING HEADERS */
		bool	CheckHeaderLineState(std::string &header_line);
		void	SaveHeaderLine(std::string req_data);

		/** SAVING AND PARSING FIRST LINE OF REQUEST */
		void	ParsePercentData(std::string &uri_ref);
		void	ParseURIData(void);
		const 	LocationData	*GetValidLocation(void);
		void	SaveProtocol(std::string &fst_line, std::size_t &space_ind);
		void	SaveURI(std::string &fst_line, std::size_t &space_ind);
		void	SaveMethod(std::string &fst_line, std::size_t &space_ind);
		void	ReadStartLine(std::string fst_line);
		void	ReadFirstBlock(std::string &req_data); /** чтение первой строки и заголовков */

/**
 * ====================================
 * ==            Variables           ==
 * ====================================
 */
		std::uint8_t									_client_fd;
		ServerKqueue									&_server_api;
		std::multimap<std::string, LocationData> const	&_allLocations;
		const LocationData								*_location;
		char											*_buffer;

		/*
		 * SAVE_REQUEST_DATA PART
		 */
		std::string							_uri;
		std::string							_uri_parameters; // строка запроса после '?'
		std::string							_method;
		std::string							_protocol;
		std::map<std::string, std::string>	_headers; // ключ:значение
		std::string							_body;
		std::uint32_t						_body_size;
		std::uint32_t						_max_body_size;
		bool								_isChunkedData; // SaveChunkedBody()
		std::uint32_t						_chunk_size; // размер пересылаемых данных
		std::string							_transfer_encoding;
		std::uint8_t						_parse_state;
		std::uint32_t						_status_code;
};

#include "Messenger.hpp"

#endif
