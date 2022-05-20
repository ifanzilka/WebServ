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
		int		*initCGI(int cgiNum, pid_t &pid);
		bool	isReadable(void){ return _toRead; }
	private:
		std::vector<char **>	makeDataForExec(std::string &path);
		void	runCgi(std::string cgiPath);
		void	changeAndCloseFd(int pos, int cgiNum);
		void	runCGIHelper(int *firstReadFromFD, int *lastSendToFD, int cgiNum);


		int										_mainFds[2];
		int										(*_pipeFds)[2];
		pid_t									_pid;
		std::map <std::string, std::string>		_reqHeaders;
		std::string								_url;
		bool									_toRead;


		std::ifstream							&_FILE;
		std::multimap<std::string, std::string>	_cgis;
		Request	&_request;
		CGI();
};


#endif
