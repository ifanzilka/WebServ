#include "Messenger.hpp"
#include <sys/socket.h>
#include <exception>

Messenger::Messenger(ServerData &server_data)
	: _server_data(server_data),
	_request(server_data.GetLocationData()),
	_response(nullptr),
	_web_page_name("index.html"),
	_root_dir("./resources"),
	_toServe(false)
{
	connectionIsClosed = false;

	_client_data = new HttpData();
	_client_data->_client_fd = 0;
	_client_data->_hasBody = false;
	_client_data->_body_length = 0;
	_status_code = 200;
	_status_line = std::to_string(_status_code) + " OK"; // TODO: CHECK IT (c++ 11)
}


Messenger::~Messenger()
{
	delete _client_data;
}

void Messenger::CollectDataForResponse()
{
	std::string file_path = DefineURLFilePath();

	printf(PURPLE"Messenger::SendResponse()\n"NORM);	//TODO: удалить
	if (file_path.empty())
	{
		_status_code = 404;
		_status_line = std::to_string(_status_code) + " Not Found";  // TODO: CHECK IT (c++ 11)
		_file_data = ReadFile("./resources/404.html", "r");
		if (_file_data.empty())
			throw RequestException(500, "Unable to open 404 PAGE!");
	}
	else
		_file_data = ReadFile(file_path, "r"); //TODO сделать распределение по типам файла (html, img, video)
}

// TODO: сделать bool возврат
void Messenger::StartMessaging(const int client_fd, std::string request_msg)
{
	//TODO: добавить время начала процесса парсинга для хедеров

	size_t read_bytes = 0;

	read_bytes = recv(client_fd, this->_request.GetBuffer(), RECV_BUFFER_SIZE, 0);
	if (read_bytes == 0)
	{
		connectionIsClosed = true;
		return ;
	}
	else if (read_bytes == -1)
		throw RequestException(502, "recv() error");

	try
	{
		/** Объект испольуется для получения информации из принятого запроса () */
//		Request request = Request();
		_client_data->_client_fd = client_fd;
		_toServe = _request.saveRequestData(read_bytes);
		// TODO: вывод статистики парсинга
		_request.PrintAllRequestData();

		_response = new Response(_request); //TODO: delete _response
		_response->SendResponse(client_fd);
//		_request.FillDataByRequest(*_client_data, std::string(buffer));

//		CollectDataForResponse();
	}
	catch (RequestException &e)
	{
		_toServe = true;
		std::cout << "REQUEST ERROR\n";
		_request.setErrorStatus(e.getStatus());
		std::cout << e.what() << std::endl;
	}

//	SendResponse();
}

//TODO:
/**
 * 1) Создать объект
 * 2) Отправить ответ,
 * 3) Убедиться в успешной отправке всех данных
 * 4) Закрыть соединение и удалить объект
 * */

void Messenger::SendResponse()
{
	if (_client_data->_http_method.compare("GET") == 0)
	{
		printf(PURPLE"GET METHOD\n"NORM);	//TODO: удалить
		GetMethod getMethod = GetMethod();
		getMethod.SendHttpResponse(_client_data->_client_fd, _file_data, _status_line, _client_data);
	}
	else if (_client_data->_http_method.compare("POST") == 0)
	{
		printf(PURPLE"POST METHOD\n"NORM);	//TODO: удалить
		PostMethod postMethod = PostMethod();
		postMethod.SendHttpResponse(_client_data->_client_fd, _file_data, _status_line, _client_data);
	}
	close(_client_data->_client_fd);
}

std::string Messenger::DefineURLFilePath()
{
	std::string fnl_file_path;
	std::string	valid_location;
	bool		file_found = false;

	printf(PURPLE"Messenger::DefineURLFilePath()\n"NORM); //TODO: удалить

	SetValidLocations();

	for (std::map<int, std::string>::iterator i = _valid_locations.begin();
		 i != _valid_locations.end(); i++)
	{
		valid_location = (*i).second;

		fnl_file_path = ConstructFullPath(valid_location);

		printf("Final path: %s\n", fnl_file_path.c_str());	//TODO: удалить
		std::ifstream file;
		file.open(fnl_file_path);
		file.close();

		if (file)
		{
			file_found = true;
			break;
		}
	}
	if (file_found == false)
		fnl_file_path.clear();
	else
	{
		printf(PURPLE"Messenger::SetValidLocations\n"NORM);
		printf("%s\n", (PURPLE"Final Path: "NORM + fnl_file_path).c_str());
	}
	return (fnl_file_path);
}

void Messenger::SetValidLocations()
{
	std::multimap<std::string, LocationData> locations = _server_data.GetLocationData();

	// TODO: удалить (проверка того, что локейшены пропарсились хорошо)
//	for (std::multimap<std::string, LocationData>::iterator it = locations.begin();
//		it != locations.end(); ++it)
//	{
//		std::cout << "PATH: " << it->first << std::endl <<
//				  it->second << std::endl;
//	}
	std::string location;
	std::multimap<std::string, LocationData>::iterator bgn = locations.begin();

	printf(PURPLE"Messenger::SetValidLocations\n"NORM); //TODO: удалить
	printf("%s\n", (BLUE"Request Path: "NORM + _client_data->_file_path).c_str()); 	//TODO: удалить
	for (; bgn != locations.end(); bgn++)
	{
		location = bgn->second.GetLocationPath();

		// если в конфиге присутствует default location
		if (location[0] == '/' && location.length() == 1)
		{
			if (_client_data->_file_path.length() == 1)
				_valid_locations.insert(std::make_pair(1, "/"));
			else
				_valid_locations.insert(std::make_pair(3, "/"));
		}
		else if (_client_data->_file_path != "/" && location.find(&_client_data->_file_path[1]) != std::string::npos)
		{
			if (bgn->second.IsExactPath() && _client_data->_file_path != location)
				continue;
			else if (!bgn->second.IsExactPath() && _client_data->_file_path != location)
				_valid_locations.insert(std::make_pair(2, location));
			else
				_valid_locations.insert(std::make_pair(1, location));
		}
	}

	//TODO: удалить
	std::multimap<int, std::string>::iterator it = _valid_locations.begin();
	for (; it != _valid_locations.end(); it++)
	{
		std::cout << (*it).first << ": " + (*it).second << std::endl;
	}
}

std::string Messenger::GetRootByLocation(std::string &location_data)
{
	std::multimap<std::string, LocationData> locations = _server_data.GetLocationData();
	std::string	root;

	std::multimap<std::string, LocationData>::iterator bgn = locations.begin();
	for (; bgn != locations.end(); bgn++)
	{
		if (bgn->first == location_data)
		{
			root = bgn->second.GetRoot();
			break;
		}
	}
	return (root);
}

std::string Messenger::ConstructFullPath(std::string valid_location)
{
	std::string file_name_after_slash = &_client_data->_file_path[_client_data->_file_path.find("/")];
	std::string fnl_file_path;

	std::string root = GetRootByLocation(valid_location);
	fnl_file_path = _root_dir;

	if (root.empty())
		printf("%s: THERE IS NO ROOT!\n", valid_location.c_str());
	else
	{
		if (root[0] == '.')
			root = &root[1];
		fnl_file_path += root;
	}

	if (fnl_file_path[fnl_file_path.length() - 1] == '/')
		fnl_file_path.resize(fnl_file_path.length() - 1);

	if (file_name_after_slash.find_last_of(".") != std::string::npos) // если в искомом пути есть расширение файла
		fnl_file_path += _client_data->_file_path;
	else
		fnl_file_path += valid_location;

	printf("file_path: %s\n", fnl_file_path.c_str());	//TODO: удалить

	// если в искомом пути и валидном location нет расширения файла
	if (file_name_after_slash.find_last_of(".") == std::string::npos
		&& (valid_location).find_last_of(".") == std::string::npos)
	{
		if (fnl_file_path[fnl_file_path.length() - 1] != '/')
			fnl_file_path += '/';
		fnl_file_path += _web_page_name;
	}
	return (fnl_file_path);
}

std::string Messenger::ReadFile(std::string file_path, std::string read_method)
{
	FILE	*file;
	size_t	file_size;
	std::string	buffer;

	file = fopen(file_path.c_str(), read_method.c_str());
	if (file != nullptr)
	{
		fseek(file, 0, SEEK_END);
		long file_length = ftell(file);
		rewind(file);

		buffer.resize(file_length);

		file_size = fread(&buffer[0], 1, file_length, file);
		fclose(file);
		printf("\033[32mMessenger successfully read \033[34m%ld\033[32m bytes from a file 👍 \033[0m\n", file_size);
	}
	else
		printf("\033[31mUnable to open file via URL!\033[0m\n");

	return (buffer);
}

void Messenger::ClearValidLocations()
{
	_valid_locations.clear();
}
