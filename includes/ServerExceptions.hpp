#ifndef WEBSERV_UFA_SERVEREXCEPTIONS_HPP
#define WEBSERV_UFA_SERVEREXCEPTIONS_HPP

#include <exception>

class RequestException : public std::exception
{
	public :

		RequestException(const char *msg)
			: errorMsg(msg), status(0)
		{}

		RequestException(int st, const char *msg)
			: errorMsg(msg), status(st)
		{}

		int getStatus() const
		{
			return status;
		}

		virtual const char* what(void) const throw ()
		{
			return (this->errorMsg);
		}

	private:

		const char*	errorMsg;
		const int	status;
};

#endif
