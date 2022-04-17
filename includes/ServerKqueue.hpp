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

namespace ft
{
	class ServerKqueue: public AbstractServerApi
	{
	public:

		/* Constructor */
		ServerKqueue(int port);
		ServerKqueue(std::string ipaddr, int port);
		ServerKqueue(const char *ipaddr, int port);

		void Start();

		/* Destructor */
		~ServerKqueue();
	private:
		
		/* Для макроса */
		struct kevent evSet;
		
		/* Очередь */
		int			_kq_fd;


		/* Init */
		void 	Init_Serv();
		
		void	CheckAccept();
		void 	AddFd(int fd);
		void	ReadFd(int fd);


	};
}

#endif