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

int	ServerKqueue::WaitEvent(int &client_fd)
{

	bzero(_evList, sizeof(_evList));
	struct timespec ts;
	ts.tv_sec = 3;
	ts.tv_nsec = 0;

	//Logger(BLUE, "Wait kevent...");
	_new_events = kevent(_kq_fd, NULL, 0, _evList, KQUEUE_SIZE, &ts);
	if (_new_events == -1)
		ServerError("kevent");

	//Logger(B_GRAY, "kevent return " + std::to_string(_new_events));
//	return (_new_events);

	int event_flag = 0;

	for (int i = 0; i < _new_events; i++)
	{
		client_fd = _evList[i].ident;

		// When the client disconnects an EOF is sent. By closing the file
		// descriptor the event is automatically removed from the kqueue.
		if (_evList[i].flags & EV_EOF)
		{
			//TODO: std::to_string - 11CPP
			Logger(RED, "Disconnect fd(" + std::to_string(client_fd) + ") ❌ ");
			RemoteFd(client_fd);
//			close(client_fd);
			event_flag = EV_EOF;
		}
		else if (client_fd == _server_fd)
		{
			client_fd = CheckAccept();
			Logger(GREEN, "Connect fd(" + std::to_string(client_fd) + ") ✅ ");
		}
		else if (_evList[i].filter & EVFILT_READ)
		{
//			ReadFd(event_fd);
			event_flag = EVFILT_READ;
		}
		else if (_evList[i].filter == EVFILT_WRITE)
		{
			//to do
			event_flag = EVFILT_WRITE;
		}
	}

	return (event_flag);
}

//TODO: CheckAccept возвращает фд клиента, но не используется нигде

/* Проверяю событие на Подключение если полюкчился возвращаю fd клиента иначе 0*/
int ServerKqueue::CheckAccept()
{
	//Logger(BLUE, "CheckAccept...");
	int client_fd;

	for (int i = 0; i < _new_events; i++)
	{
		int event_fd = _evList[i].ident;

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

	for (int i = 0; i < _new_events; i++)
	{
		int event_fd = _evList[i].ident;

		// When the client disconnects an EOF is sent. By closing the file
		// descriptor the event is automatically removed from the kqueue.
		if (_evList[i].flags & EV_EOF)
		{
			Logger(RED, "Disconnect fd(" + std::to_string(event_fd) + ") ❌ ");
			RemoteFd(event_fd); //TODO: переименовать в Remove
		}
		else if (_evList[i].filter & EVFILT_READ)
		{
			return (event_fd);
			//ReadFd(event_fd);
		}
	}
	return (0);
}

void ServerKqueue::disableReadEvent(int socket, void *udata)
{
	struct kevent kv;
	EV_SET(&kv, socket, EVFILT_READ, EV_DISABLE, 0 , 0, udata);
	if (kevent(_kq_fd, &kv, 1, NULL, 0, NULL) == -1)
	{
		std::cerr << "ERROR disabling event for read" << std::endl;
	}
}

void ServerKqueue::enableWriteEvent(int socket, void *udata)
{
	struct kevent kv;
	EV_SET(&kv, socket, EVFILT_WRITE, EV_ENABLE, 0 , 0, udata);
	if (kevent(_kq_fd, &kv, 1, NULL, 0, NULL) == -1)
	{
		std::cerr << "ERROR enabling event for write" << std::endl;
	}
}

void ServerKqueue::disableWriteEvent(int socket, void *udata)
{
	struct kevent kv;
	EV_SET(&kv, socket, EVFILT_WRITE, EV_DISABLE, 0 , 0, udata);
	if (kevent(_kq_fd, &kv, 1, NULL, 0, NULL) == -1)
	{
		std::cerr << "ERROR disabling event for write" << std::endl;
	}
}

void ServerKqueue::addReadEvent(int socket, void *udata)
{
	struct kevent kv;
	EV_SET(&kv, socket, EVFILT_READ, EV_ADD, 0 , 0, udata);
	if (kevent(_kq_fd, &kv, 1, NULL, 0, NULL) == -1)
	{
		std::cerr << "ERROR adding event for write" << std::endl;
	}
//	update_len++;
}

void ServerKqueue::addWriteEvent(int socket, void *udata)
{
	struct kevent kv;
	EV_SET(&kv, socket, EVFILT_WRITE, EV_ADD , 0 , 0, udata);
	if (kevent(_kq_fd, &kv, 1, NULL, 0, NULL) == -1)
	{
		std::cerr << "ERROR adding event for write" << std::endl;
	}
//	update_len++;
}

/**
 * Добавляет отслеживаемые события в kqueue:
 * 1) чтение
 * 2) запись
 * 3) временно отключает сигнал о готовности записи
 * */
void ServerKqueue::AddFd(int fd)
{
	Logger(B_GRAY, "Add fd " + std::to_string(fd));

	addReadEvent(fd, this);
	addWriteEvent(fd, this);
	disableWriteEvent(fd, this);
}

//TODO: переименовать в Remove
void ServerKqueue::RemoteFd(int client_fd)
{
	Logger(B_GRAY, "Remote fd " + std::to_string(client_fd)); //TODO: переименовать в Remove

	close(client_fd);
	RemoteClient(client_fd); //TODO: переименовать в Remove
}

/* Destrcutor */
ServerKqueue::~ServerKqueue()
{
	close(_kq_fd);
	Logger(RED, "Call ServerKqueue Destructor ❌ ");
}