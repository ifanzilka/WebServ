#ifndef SERVER_KQUEUE
# define SERVER_KQUEUE

/* Abstract class */
#include "ServerApi.hpp"

#include "Color.hpp"	/* Color */

/* kqueue */
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#define 		KQUEUE_SIZE 64

#include <Include_Library.hpp>
#include "../Messenger.hpp"

class ServerData;

class ServerKqueue: public AbstractServerApi
{
	public:

		/* Constructor */
		ServerKqueue(int port);
		ServerKqueue(std::string ipaddr, int port);
		ServerKqueue(const char *ipaddr, int port);

		virtual int			WaitEvent();
		virtual int			CheckAccept();
		virtual	int 		CheckRead();
		//virtual int 		CheckWrite();
		//virtual	int			ReadFd(int fd);

		/* Destructor */
		virtual ~ServerKqueue();


	private:
		struct kevent 	evList[KQUEUE_SIZE];
		int	 			new_events;
		int				client_fd;

		/* Для макроса */
		struct kevent evSet;

		/* Очередь */
		int			_kq_fd;


		/* Init */
		void 	Init_Serv();
		void 	AddFd(int fd);
		void	RemoteFd(int client_fd);
};

#endif