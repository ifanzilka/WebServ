#ifndef SERVER_KQUEUE
# define SERVER_KQUEUE

#include "Common_Header.hpp"

#define 		KQUEUE_SIZE 64

class ServerData;

class ServerKqueue: public AbstractServerApi
{
	public:

		/* Constructor */
		ServerKqueue(int port);
		ServerKqueue(std::string ipaddr, int port);
		ServerKqueue(const char *ipaddr, int port);

		virtual int			WaitEvent(int &client_fd);
		virtual int			CheckAccept();
		virtual	int 		CheckRead();
		//virtual int 		CheckWrite();
		//virtual	int			ReadFd(int fd);

		/* Destructor */
		virtual ~ServerKqueue();

		void	disableReadEvent(int socket, void *udata);
		void	enableWriteEvent(int socket, void *udata);
		void	disableWriteEvent(int socket, void *udata);
	private:
		void			addWriteEvent(int socket, void *udata);
		void			addReadEvent(int socket, void *udata);
		struct kevent 	_evList[KQUEUE_SIZE];
		int	 			_new_events;
		int				client_fd;

		/* Для макроса */
		struct kevent evSet;

		/* Очередь */
		int			_kq_fd;


		/* Init */
		void 	Init_Serv();
		void 	AddFd(int fd);
		void	RemoteFd(int client_fd); //TODO: переименовать в Remove
};

#endif