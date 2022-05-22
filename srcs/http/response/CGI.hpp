//
// Created by Josephine Beregond on 5/15/22.
//

#ifndef CGI_HPP
#define CGI_HPP

#include "Common_Header.hpp"
class Request;

class CGI
{
	public:
		~CGI();
		CGI(Request &_request, const std::multimap<std::string, std::string> &cgis,
			std::ifstream &FILE);

		void		toRead(bool flag) { _toRead = flag; }
		const int	*CGI_Init(const int &cgi_amnt, pid_t &pid);
		bool		isReadable(void) { return _toRead; }
	private:
		std::vector<char **>	MakeDataForExec(const std::string &path);
		void					RunCgi(const std::string &cgi_path);
		void					ChangeAndCloseFd(int pos, const int &cgi_num);
		void					RunCGIHelper(const int *firstReadFromFD, const int *lastSendToFD, const int &cgi_num);

		int										_mainFds[2];
		int										(*_pipeFds)[2];
		pid_t									_pid;
		std::map <std::string, std::string>		_req_headers;
		std::string								_url;
		bool									_toRead;

		std::ifstream							&_FILE;
		std::multimap<std::string, std::string>	_cgis;
		Request									&_request;
};


#endif
