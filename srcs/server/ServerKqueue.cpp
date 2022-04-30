#include <Include_Library.hpp>
//#include "ServerKqueue.hpp"

/* Constructor */
ServerKqueue::ServerKqueue(int port)
{
	std::string tmp = "127.0.0.1";
	AbstractServerApi::Init(tmp, port);
	Init_Serv();
}

ServerKqueue::ServerKqueue(const char *ipaddr, int port)
{
	std::string tmp = std::string(ipaddr);;
	AbstractServerApi::Init(tmp, port);
	Init_Serv();
}

ServerKqueue::ServerKqueue(std::string ipaddr, int port)
{
	AbstractServerApi::Init(ipaddr, port);
	Init_Serv();
}

/* Init */
void 	ServerKqueue::Init_Serv()
{
	_logs << "ServerType: Kqueue 🌐 " << std::endl;

	int res_kevent;

	_kq_fd = kqueue();
	if (_kq_fd == -1)
		ServerError("Kqueue");

	/* Добавляю событие прослушки fd сервера */
	/* Задаем события для отслеживания */
	EV_SET(&evSet, _server_fd, EVFILT_READ, EV_ADD | EV_ENABLE , 0, 0, NULL);

	res_kevent = kevent(_kq_fd, &evSet, 1, NULL, 0, NULL);
	if (res_kevent == -1)
		ServerError("Kevent");
	bzero(&evSet,sizeof(evSet));
}

int	ServerKqueue::WaitEvent()
{
	bzero(evList, sizeof(evList));
	struct timespec ts;
	ts.tv_sec = 3;
	ts.tv_nsec = 0;

	Logger(BLUE, "Wait kevent...");
	new_events = kevent(_kq_fd, NULL, 0, evList, KQUEUE_SIZE, &ts);
	if (new_events == -1)
		ServerError("kevent");

	Logger(B_GRAY, "kevent return " + std::to_string(new_events));
	return (new_events);

	// for (int i = 0; i < new_events; i++)
	// {
	// 	int event_fd = evList[i].ident;

	// 	// When the client disconnects an EOF is sent. By closing the file
	// 	// descriptor the event is automatically removed from the kqueue.
	// 	if (evList[i].flags & EV_EOF)
	// 	{
	// 		Logger(RED, "Disconnect fd(" + std::to_string(event_fd) + ") ❌ ");
	// 		close(event_fd);
	// 	}
	// 	else if (event_fd == _server_fd)
	// 	{
	// 		CheckAccept();
	// 	}
	// 	else if (evList[i].filter & EVFILT_READ)
	// 	{
	// 		ReadFd(event_fd);
	// 	}
	// 	else if (evList[i].filter == EVFILT_WRITE)
	// 	{
	// 		//to do
	// 	}

	// }
}

/* Проверяю событие на Подключение если полюкчился возвращаю fd клиента иначе 0*/
int ServerKqueue::CheckAccept()
{
	Logger(BLUE, "CheckAccept...");
	int client_fd;

	for (int i = 0; i < new_events; i++)
	{
		int event_fd = evList[i].ident;

		if (event_fd == _server_fd)
		{
			break;
		}
		return (0);
	}

	client_fd = Accept();
	if (client_fd == -1)
	{
		//ServerError("CheckAccept");
		return -1;
	}
	AddFd(client_fd);
	return (client_fd);
}

/* Проверяю на чтение если нету возвращаю 0, иначе fd откуда читать */
int ServerKqueue::CheckRead()
{
	Logger(BLUE, "Check Read ...");

	for (int i = 0; i < new_events; i++)
	{
		int event_fd = evList[i].ident;

		// When the client disconnects an EOF is sent. By closing the file
		// descriptor the event is automatically removed from the kqueue.
		if (evList[i].flags & EV_EOF)
		{
			Logger(RED, "Disconnect fd(" + std::to_string(event_fd) + ") ❌ ");
			RemoteFd(event_fd);
		}
		else if (evList[i].filter & EVFILT_READ)
		{
			return (event_fd);
			//ReadFd(event_fd);
		}
	}
	return (0);
}

void ServerKqueue::AddFd(int fd)
{
	Logger(B_GRAY, "Add fd " + std::to_string(fd));

	/* Добавляю нового клиента в аулл фд*/
	EV_SET(&evSet, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(_kq_fd, &evSet, 1, NULL, 0, NULL) < 0)
	{
		ServerError("kevent");
	}
}

void ServerKqueue::RemoteFd(int client_fd)
{
	Logger(B_GRAY, "Remote fd " + std::to_string(client_fd));

	close(client_fd);
	RemoteClient(client_fd);
}

/* Destrcutor */
ServerKqueue::~ServerKqueue()
{
	close(_kq_fd);
	Logger(RED, "Call ServerKqueue Destructor ❌ ");
}