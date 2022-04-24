#include "Messenger.hpp"
#include <sys/socket.h>
#include <exception>

#include "./server/Color.hpp"

namespace ft
{
	Messenger::Messenger(ServerData &server_data)
		: _server_data(server_data), _web_page_name("index.html"),
			_root_dir("./resources")
	{}


	Messenger::~Messenger() {}

	void Messenger::SetClientFd(const int client_fd)
	{
		this->_client_fd = client_fd;
	}

	int Messenger::SetRequest(const int client_fd, std::string request)
	{
		int rv = 1;
		ft::RequestParser requestParser = ft::RequestParser();

		if (request.empty()
			|| (_http_method = requestParser.GetHttpMethod(request)).empty())
			rv = -1;
		else
		{
			SetClientFd(client_fd);
			_file_path = requestParser.GetFilePath(request);
			//TODO: убедиться в том, что версия HTTP протокола нам не нужна
			// (строка ниже удаляет эту информацию)
			request.erase(0, request.find("\n") + 1);
			_headers = requestParser.GetHeaders(request);

			SendResponse();
		}
		return (rv);
	}

	void Messenger::SetValidLocations()
	{
		std::vector <LocationData> locations = _server_data.GetLocationData();
		std::string location;

		std::vector<LocationData>::iterator bgn = locations.begin();

		printf("Messenger::SetValidLocations\n");
		printf("%s\n", (BLUE"Request Path: "NORM + _file_path).c_str());
		for (; bgn != locations.end(); bgn++)
		{
			location = (*bgn).GetLocationPath();
			// первый if проверяет ищем ли мы файл с разрешением(.html)
			// или же просто открываем корень
			if ((_file_path.length() == 1 && location[0] == '/')
				|| _file_path.find_first_of(".") != std::string::npos)
			{
				_valid_locations.insert(std::make_pair(1, "/"));
				break;
			}
			else if (location.length() > 1 && location.find(&_file_path[1]) != std::string::npos)
			{
				if ((*bgn).IsExactPath() && _file_path != location)
					continue;
				else if (!(*bgn).IsExactPath() && _file_path != location)
					_valid_locations.insert(std::make_pair(2, location));
				else
					_valid_locations.insert(std::make_pair(1, location));
			}
		}

		std::multimap<int, std::string>::iterator it = _valid_locations.begin();
		for (; it != _valid_locations.end(); it++)
		{
			std::cout << (*it).first << ": " + (*it).second << std::endl;
		}
	}

	//TODO: добавить ./resources в root как дефолтное значение
	// и проверять наличие файла для цикла for
	std::string Messenger::DefineURLFilePath()
	{
		std::string fnl_file_path;
		std::string	vld_location;
		bool		file_found = false;

		SetValidLocations();

		for (std::map<int, std::string>::iterator i = _valid_locations.begin();
			i != _valid_locations.end(); i++)
		{
			vld_location = (*i).second;
			std::string file_name_after_slash = &_file_path[_file_path.find("/")];
			fnl_file_path = _root_dir;

			if (file_name_after_slash.find_last_of(".") != std::string::npos) // если в искомом пути есть расширение файла
				fnl_file_path += _file_path;
			else
				fnl_file_path += vld_location;

			printf("file_path: %s\n", fnl_file_path.c_str());
			printf("file_path_slash: %s\n", file_name_after_slash.c_str());

			// если в искомом пути и валидном location нет расширения файла
			if (file_name_after_slash.find_last_of(".") == std::string::npos
				&& (vld_location).find_last_of(".") == std::string::npos)
				fnl_file_path += ("/" + _web_page_name);
			printf("Final path: %s\n", fnl_file_path.c_str());
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
			printf("Messenger::SetValidLocations\n");
			printf("%s\n", (PURPLE"Final Path: "NORM + fnl_file_path).c_str());
		}
		return (fnl_file_path);
	}

	void Messenger::SendResponse()
	{
		std::vector<char> file_buffer;
		std::string	http_code = "200 OK";
		std::string file_path = DefineURLFilePath();

		printf("Messenger::SendResponse()\n");
		printf("FILE_PATH before empty(): '%s'\n", file_path.c_str());
		if (file_path.empty())
		{
			http_code = "404 Not Found";
			file_buffer = ReadFile("./resources/404.html", "r");
			if (file_buffer.empty())
			{
				printf("Messenger::SendResponse\n");
				printf("\033[31mUnable to open 404.html! 😔 \033[0m\n");
				throw std::runtime_error("\033[31mUnable to open 404.html! \033[0m\n");
			}
		}
		else
			file_buffer = ReadFile(file_path, "r"); //TODO сделать распределение по типам файла (html, img, video)

		if (_http_method.compare("GET") == 0)
		{
			GetMethod getMethod = GetMethod();
			getMethod.SendHttpResponse(_client_fd, file_buffer, http_code);
		}
	}

	std::vector<char> Messenger::ReadFile(std::string file_path, std::string read_method)
	{
		FILE	*file;
		size_t	file_size;
		std::vector<char>	buffer;

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

}