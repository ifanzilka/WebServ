#ifndef DEMULTIPLEXER_HPP
#define DEMULTIPLEXER_HPP

#include <string>
#include <Include_Library.hpp>

class ServerCore
{
	public:
		ServerCore(const char *processing_method);
		~ServerCore();
		void Start() const;
	private:
		std::string _processing_method;
		void	StartWebServer() const;
};

#endif
