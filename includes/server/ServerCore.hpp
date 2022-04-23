#ifndef DEMULTIPLEXER_HPP
#define DEMULTIPLEXER_HPP

#include <string>
#include <Include_Library.hpp>

class ServerCore
{
	public:
		ServerCore(std::string &processing_method, ServerData &server_data);
		~ServerCore();
		void Start() const;

	private:
		ServerData 	&_server_data;
		std::string	_processing_method;
		void	StartWebServer() const;
};

#endif
