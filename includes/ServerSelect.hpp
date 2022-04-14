#ifndef SERVER_SELECT
# define SERVER_SELECT

#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>	/* inet_addr inet_htop */
#include <sys/socket.h>	/* socket */
#include <sys/select.h>	/* select */

/* Abstract Class  */
#include "ServerApi.hpp"
#include "Color.hpp"

#include <iostream> /* string */
#include <vector>	/* vector */
#include <map>		/* map	*/
#include <unistd.h> /* write */

#define SERVER_PROTOCOL 	AF_INET 	//IpV4
#define	SERVER_TYPE			SOCK_STREAM //TCP
#define MAX_CONNECT_LISTEN 	15			//In Listen
#define	BUFFER_SIZE			1024		//In Read Buffer


namespace ft
{
	class ServerSelect//: public AbstractServerApi
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
		/* Настройка моей сети */
		struct sockaddr_in 	_servaddr;
		std::string 		_ipaddr;
		
		/* Fd and id clients */
		std::map<int, int>	_clients_fd;
		
		int 				_port;
		int 				_server_fd;
		int					_max_fd;
		int					_id;

		/* For select */
		fd_set				_currfds;
		fd_set 				_writefds;
		fd_set 				_readfds;


		void 	PrintIpPort();
		void 	PrintClientInfo(struct sockaddr_in *info);
		
		/* Print Errno */
		void	ServerError(const char *s);
		
		
		void 	Init();
		int		Create_socket();
		int 	Bind();
		int 	Listen();

		/* Events */
		void 	EventsCheck();
		void 	CheckListen();
		void	CheckRead();
		void 	CheckWrite();

		void 	ReadFd(int clinet_fd);


		void	AddClient(int fd);
		void 	DeleteClient(int fd);
		void 	PrintAllClients();

	};
}

#endif