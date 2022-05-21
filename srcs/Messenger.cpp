#include "Messenger.hpp"
#include <sys/socket.h>
#include <exception>
#include "server/ServerKqueue.hpp"

Messenger::Messenger(ServerData &server_data, ServerKqueue &server_api)
	: _server_data(server_data),
	_server_api(server_api),
	_request(server_data.GetLocationData()),
	_response(nullptr),
	_toServe(false)
{
	isClosed = false;
	_client_fd = 0;

//	_client_data = new HttpData();
//	_client_data->_client_fd = 0;
//	_client_data->_hasBody = false;
//	_client_data->_body_length = 0;
}

Messenger::~Messenger()
{
	delete _client_data;
}

// TODO: сделать bool возврат
void Messenger::ReadRequest(const int client_fd)
{
	//TODO: добавить время начала процесса парсинга для хедеров
	_client_fd = client_fd;
	size_t read_bytes = 0;

	read_bytes = recv(_client_fd, this->_request.GetBuffer(), RECV_BUFFER_SIZE, 0);
	if (read_bytes == 0)
	{
		isClosed = true;
		return ;
	}
	else if (read_bytes == -1)
		throw RequestException(502, "recv() error");

	try
	{
		/** Объект испольуется для получения информации из принятого запроса () */
//		_client_data->_client_fd = client_fd;
		_toServe = _request.saveRequestData(read_bytes);
		// TODO: вывод статистики парсинга
		_request.PrintAllRequestData();

		if (_toServe)
		{
			_server_api.disableReadEvent(client_fd, &_server_api);
//			_server_api.enableWriteEvent(client_fd, &_server_api);
		}

//		MakeResponse();
	}
	catch (RequestException &e)
	{
		_toServe = true;
		_request.setErrorStatus(e.getStatus());
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
			_toServe = false;
			delete _response;
			_response = nullptr;
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		_toServe = false;
		isClosed = true;
	}
}

void Messenger::ClearValidLocations()
{
	_valid_locations.clear();
}
