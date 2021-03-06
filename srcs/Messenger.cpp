#include "Messenger.hpp"
#include <sys/socket.h>
#include <exception>
#include "server/ServerKqueue.hpp"

Messenger::Messenger(ServerData &server_data, ServerKqueue &server_api)
	: _server_api(server_api),
	_request(server_data.GetLocationData(), server_api),
	_response(nullptr)
{
	_client_fd = 0;
}

Messenger::~Messenger()
{}

// TODO: сделать bool возврат
void Messenger::ReadRequest(const int &client_fd)
{
	size_t read_bytes = 0;

	//TODO: добавить время начала процесса парсинга для хедеров
	this->_client_fd = client_fd;
	_request.SetClientFd(client_fd);

	read_bytes = recv(_client_fd, this->_request.GetBuffer(), RECV_BUFFER_SIZE, 0);
	if (read_bytes == 0)
		return;
	else if (read_bytes == -1)
		throw RequestException(502, "recv() error");

	try
	{
		_request.ReadRequestData(read_bytes);
//		_request.PrintAllRequestData(); // TODO: вывод статистики парсинга
	}
	catch (const RequestException &e)
	{
		_request.SetStatusCode(e.getStatus());
		std::cout << e.what() << std::endl;
	}
}

void Messenger::MakeResponse()
{
	try
	{
		if (!_response)
			_response = new Response(_request);
		_response->SendResponse(_client_fd);
		if (_response->isSent())
		{
			_server_api.disableWriteEvent(_client_fd, &_server_api);
			delete _response;
			_response = nullptr;
		}
	}
	catch (const RequestException &e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void Messenger::ClearValidLocations()
{
	_valid_locations.clear();
}
