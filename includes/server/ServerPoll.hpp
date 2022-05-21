#ifndef SERVER_POLL
#define SERVER_POLL

#include "ServerApi.hpp"
#include <poll.h>		/* poll */


#include <vector>

class ServerPoll: public AbstractServerApi
{
	public:

		/* Constructor */
		ServerPoll(int port);
		ServerPoll(std::string &ipaddr, int port);
		ServerPoll(const char *ipaddr, int port);

		virtual int			WaitEvent(int &client_fd);
		virtual int			CheckAccept();
		virtual	int 		CheckRead();
		//virtual int 		CheckWrite();
		virtual	int			ReadFd(int fd);

		/* Destructor */
		virtual ~ServerPoll();

	private:
		/* Client fds */
		std::vector<struct pollfd> _pollfds;

		/* Init Serv */
		void 	Init_Serv();

		void 	AddFd(int fd);
		void	RemoveFd(int client_fd);
};

#endif