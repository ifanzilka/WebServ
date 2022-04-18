#ifndef SERVER_SELECT
# define SERVER_SELECT

/* Abstract Class  */
#include "ServerApi.hpp"
#include "Color.hpp"

#include <iostream> /* string */
#include <vector>	/* vector */
#include <map>		/* map	*/
#include <unistd.h> /* write */

namespace ft
{
	class ServerSelect: public AbstractServerApi
	{
	public:

		/* Constructors */
		ServerSelect(int port);
		ServerSelect(std::string &ipaddr, int port);
		ServerSelect(const char *ipaddr, int port);

		void Start();

		/* Destructor */
		~ServerSelect();
	
	private:
		/* Fd and id clients */
		std::map<int, int>	_clients_fd;
		

		int					_max_fd;
		int					_id;

		/* For select */
		fd_set				_currfds;
		fd_set 				_writefds;
		fd_set 				_readfds;


		void 	Init_Serv();
		
		void 	PrintClientInfo(struct sockaddr_in *info);
		
	
		/* Events */
		void 	EventsCheck();
		void 	CheckAccept();
		void	CheckRead();
		void 	CheckWrite();

		void	AddFd(int client_fd);
		void	RemoteFd(int client_fd);
		


		void 	ReadFd(int clinet_fd);
		void 	PrintAllClients();

	};
}

#endif