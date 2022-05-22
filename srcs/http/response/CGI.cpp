//
// Created by Josephine Beregond on 5/19/22.
//

#include "CGI.hpp"

CGI::CGI(Request &request, const std::multimap<std::string, std::string> &cgis,
	std::ifstream &FILE)
		: _request(request), _cgis(cgis), _FILE(FILE)
{
	std::uint32_t	status;

	_toRead = false;
	status = 200;
	_url = _request.GetUrl(status);
	_req_headers = _request.GetHeaders();
	request.GetUrlEncodedBody(_req_headers); //TODO: проверить внимательно (ссылку инициализируется значения)

	_req_headers.insert(std::pair<std::string, std::string>("QUERY_STRING",
		_request.GetURIParameters()));
	_req_headers.insert(std::pair<std::string, std::string>("REQUEST_METHOD",
		_request.GetMethod()));
	_req_headers.insert(std::pair<std::string, std::string>("PATH_INFO", _url));
	_req_headers.insert(std::pair<std::string, std::string>("PATH_TRANSLATED",
		_request.GetLocation()->GetRoot()));
	_req_headers.insert(std::pair<std::string, std::string>("SERVER_PROTOCOL", "HTTP/1.1"));
}

CGI::~CGI() {}

std::vector<char **>	CGI::MakeDataForExec(const std::string &path)
{
	char **env = new char*[_req_headers.size() + 1];
	char **args = new char*[2];
	std::vector<char **>	to_ret;

	env[_req_headers.size()] = nullptr;
	args[1] = nullptr;
	args[0] = strdup(path.c_str());
	std::map <std::string, std::string>::iterator i = _req_headers.begin();
	for (int j = 0; i != _req_headers.end(); i++, j++)
		env[j] = strdup((i->first + "=" + i->second).c_str());
	to_ret.push_back(args);
	to_ret.push_back(env);
	return to_ret;
}

void	CGI::RunCgi(const std::string &cgi_path)
{
	std::vector<char **>	execve_args;

	execve_args = MakeDataForExec(cgi_path);
	execve(execve_args[0][0], execve_args[0], execve_args[1]);
	exit(EXIT_FAILURE);
}

void	killChilds(pid_t *pid, int childNum)
{
	for (int i = 0; i < childNum; i++)
		kill(pid[i], SIGKILL);
}

void	CGI::ChangeAndCloseFd(int pos, const int &cgi_num)
{
	if (!pos)
	{
		if (dup2(_mainFds[0], STDIN_FILENO) == -1)
		{
			exit(EXIT_FAILURE);
		}
	}
	if (pos > 0)
	{
		if (dup2(_pipeFds[pos - 1][0], STDIN_FILENO) == -1)
			exit(EXIT_FAILURE);
		close(_pipeFds[pos - 1][0]), close(_pipeFds[pos - 1][1]);
	}
	if (pos + 1 < cgi_num)
	{
		if (dup2(_pipeFds[pos][1], STDOUT_FILENO) == -1)
			exit(EXIT_FAILURE);
		close(_pipeFds[pos][0]), close(_pipeFds[pos][1]);
	}
	if (pos + 1 == cgi_num)
	{
		if (dup2(_mainFds[1], STDOUT_FILENO) == -1)
			exit(EXIT_FAILURE);
	}
	close(_mainFds[0]), close(_mainFds[1]);
}

void	CGI::RunCGIHelper(const int *firstReadFromFD, const int *lastSendToFD, const int &cgi_num)
{
	int		iter;
	pid_t	cgiPids[cgi_num];
	int		status;
	int		ret;

	ret = 0;
	_pipeFds = new int[cgi_num][2];

	_mainFds[0] = firstReadFromFD[0];
	_mainFds[1] = lastSendToFD[1];

	std::multimap<std::string, std::string> :: iterator i = _cgis.begin();
	iter = -1;
	while (++iter < cgi_num && i != _cgis.end())
	{
		if (iter + 1 < cgi_num)
		{
			if (pipe(_pipeFds[iter]) == -1)
				exit(EXIT_FAILURE);
		}
		cgiPids[iter] = fork();
		if (cgiPids[iter] == -1)
		{
			killChilds(cgiPids, iter);
			exit(EXIT_FAILURE);
		}
		if (!cgiPids[iter])
		{
			ChangeAndCloseFd(iter, cgi_num);
			close(firstReadFromFD[1]);
			close(lastSendToFD[0]);
			RunCgi(i->second);
		}
		if (iter)
			close(_pipeFds[iter - 1][0]), close(_pipeFds[iter - 1][1]);
		i++;
	}
	close(firstReadFromFD[0]);
	close(firstReadFromFD[1]);
	close(lastSendToFD[0]);
	close(lastSendToFD[1]);

	for (int i = 0; i < cgi_num; i++)
	{
		waitpid(cgiPids[i], &status, 0);
		std::cout << "PROC #" << i << " EXITED WITH CODE " << status << std::endl;
		if (status != 0)
			ret = status;
	}
	if (!ret)
		exit(EXIT_SUCCESS);
	exit(EXIT_FAILURE);
}

/*	cgiFds[0] - fd to read data from the last cgi script
	cgiFds[1] - fd to send data to the first cgi script	*/
const int	*CGI::CGI_Init(const int &cgi_amnt, pid_t &pid)
{
	int	*cgi_fds;
	int	mainFD[2][2];

	if (cgi_amnt < 1)
		throw RequestException(502, "cgiNum incorrect");
	if (pipe(mainFD[0]) == -1 or pipe(mainFD[1]) == -1)
		throw RequestException(502, "pipes");
	_pid = fork();
	if (_pid == -1)
		throw RequestException(502, "fork");
	if (!_pid)
		RunCGIHelper(mainFD[0], mainFD[1], cgi_amnt);

	close(mainFD[0][0]);
	close(mainFD[1][1]);

	cgi_fds = new int[2];
	cgi_fds[1] = mainFD[0][1];
	cgi_fds[0] = mainFD[1][0];
	pid = _pid;

	return (cgi_fds);
}