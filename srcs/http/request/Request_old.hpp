//
// Created by Josephine Beregond on 4/30/22.
//

#ifndef REQUEST_H
#define REQUEST_H

#include "./Common_Header.hpp"

struct HttpData;

class Request
{
	public:
		Request(std::multimap<std::string, LocationData> &locations);
		~Request();
		void			FillDataByRequest(HttpData &client_data, std::string request_text);
		char			*GetBuffer(void) const;

	private:

		std::multimap<std::string, LocationData> const	&_allLocations;
		const LocationData								*_location;
		char											*_buffer;

		void			CheckFirstLineSyntax(std::string &first_line);
		void			CheckProtocol(const std::string &protocol_info);
		void			CheckHeaders(HttpData &client_data);

		std::string		GetHttpMethod(std::string &request);
		std::string		GetFilePath(std::string &request);
		std::string		GetProtocol(std::string &request);
		void			FillHeadersMap(std::string &request, HttpData &client_data);
		size_t			GetHeaderEndPos(std::string &client_request);
		void			FillBodyData(HttpData &client_data, std::string &request_text);

};

#include "Messenger.hpp"

#endif
