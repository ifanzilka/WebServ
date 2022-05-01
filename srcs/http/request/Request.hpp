//
// Created by Josephine Beregond on 4/30/22.
//

#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <vector>
#include <unordered_map>


struct HttpData;

class Request
{
	public:
		void						FillDataByRequest(HttpData &client_data, std::string request_text);
	private:
		void						CheckFirstLineSyntax(std::string &first_line);
		void						CheckProtocol(const std::string &protocol_info);
		std::string					GetHttpMethod(std::string &request);
		std::string					GetFilePath(std::string &request);
		std::string					GetProtocol(std::string &request);
		void						FillHeadersMap(std::string &request, HttpData &client_data);
		size_t						GetHeaderEndPos(std::string &client_request);
};

#include "Messenger.hpp"

#endif
