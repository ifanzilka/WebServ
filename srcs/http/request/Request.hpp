//
// Created by Josephine Beregond on 4/30/22.
//

#ifndef REQUEST_H
#define REQUEST_H

#include "./Common_Header.hpp"
#include "tools.hpp"

# define START_LINE		0
# define HEADER_LINE	1
# define BODY_LINE		2
# define END_STATE		3
# define HTTP_PROTOCOL	"HTTP/1.1"

struct HttpData;

class Request
{
	public:
		Request(std::multimap<std::string, LocationData> &locations);
		~Request();

		/**
		* RESPONSE_DATA PART
		*/
		std::string							getBody(void);
		const LocationData					*getLocation() const;
		std::string							validateUrl(std::string &fullPath, std::uint32_t &status, std::uint8_t mode);
		std::string							getUrl(std::uint32_t &status);
		std::string							getMethod(void) const;
		std::map<std::string, std::string>	&getHeaders(void);
		uint32_t							getErrorStatus(void) const;

		/**
		* SAVE_REQUEST_DATA PART
		*/
		void	PrintAllRequestData();
		void	setErrorStatus(const int s);
		void	saveSimpleBody(std::string &data);
		void	parseChunkedBody(std::string &data);
		void	parseChunkSize(std::string &data);
		void	saveChunkedBody(std::string &data);
		void	saveHeaderLine(std::string headerLine);
		void	parsePercent(std::string &strRef);
		void	parseUri(void);
		const 	LocationData	*getLoc(void);
		void	validateStartLine(void);
		void	saveStartLine(std::string startLine);
		bool	saveRequestData(size_t data_size);
		char	*GetBuffer(void) const;

	private:
		void	saveStartLineHeaders(std::string &req_data);


		std::multimap<std::string, LocationData> const	&_allLocations;
		const LocationData								*_location;
		char											*_buffer;

		/*
		 * SAVE_REQUEST_DATA PART
		 */
		std::string									_body;
		bool										_isReqDone; // флаг означающий, что сохранение тела закончено
		std::uint32_t								_chunkSize; // размер пересылаемых данных
		bool										_isChunkSize; // saveChunkedBody()
		std::uint32_t								_bodySize;
		std::string									_transferEncoding;
		std::map<std::string, std::string>			_headers; // ключ:значение
		std::string									_query; // строка запроса после '?'
		std::uint32_t								_maxBodySize;
		std::uint8_t								_parseState;
		std::string									_tmpBuffer;
		std::string									_method;
		std::string									_protocol;
		std::string									_uri;
		uint32_t									_errorStatus;
};

#include "Messenger.hpp"

#endif
