#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <string>
#include <vector>

namespace ft
{
	class RequestParser
	{
		public:
			std::string					GetHttpMethod(std::string &request);
			std::string					GetFilePath(std::string &request);
			std::vector<std::string>	GetHeaders(std::string &request);
		private:
			size_t						GetHeaderEndPos(std::string &client_request);
	};
}

#endif
