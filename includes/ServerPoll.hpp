#ifndef SERVER_POLL
#define SERVER_POLL

#include "ServerApi.hpp"
#include <poll.h>		/* poll */


#include <vector>


namespace ft
{
	class ServerPoll: public AbstractServerApi
	{
	public:
		//typedef std::vector<struct pollfd>	pollfd_arr;

		/* Constructor */
		ServerPoll(int port);
		ServerPoll(std::string &ipaddr, int port);
		ServerPoll(const char *ipaddr, int port);
		
		void 	Start();


		/* Destructor */
		~ServerPoll();

	private:
		void 	Init_Serv();

		/* Client fds */
		//pollfd_arr	_pollfds;
		std::vector<struct pollfd> _pollfds;


		void 	EventsCheck();
		void 	CheckAccept();
		void	CheckRead();
		void	ReadFd(int fd);

		void 	AddFd(int fd);
		void	RemoteFd(int client_fd);

	};
}

#endif