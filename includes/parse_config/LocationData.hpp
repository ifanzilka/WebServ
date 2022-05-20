#ifndef LOCATIONDATA_HPP
#define LOCATIONDATA_HPP

#include <iostream>
#include <vector>
#include <set>
#include <sstream>

/* данные содержащиеся в локейшене сервера (у сервера может быть несколько локейшенов) */

class LocationData 
{
	private:
		std::string 				_location_path;
		// std::string 				_path;
		std::string 				_root;
		std::string 				_full_path;
		std::multimap<std::string, std::string> _cgi;
		std::string					_cgi_path;
		std::string					_cgi_extension;
		std::string 				_index;
		std::string 				_redirect;
		std::vector<std::string>	_methods;
		bool						_autoindex;
		bool						_exact_path;

		/**
		 	_client_buffer_body_size:
			Сравниваем размер тела из запроса для ответа.
			Если тело в запросе request больше, чем 100 символов,
		 	то в ответе возвращается ошибка - посмотреть номер в http
		*/
		size_t					_client_buffer_body_size;

	public:
		LocationData();
		~LocationData();
		LocationData(const LocationData &copy);

		/* Getters */
		bool							IsAutoindex() const;
		bool							IsExactPath() const;
		std::string const				&GetLocationPath() const;
		size_t							GetClientBufferBodySize() const;
		const std::string				&GetRoot() const;
		const std::string				&GetFullPath() const;
		const std::multimap<std::string, std::string> &getCgi() const;
		const std::string				&GetCgiPath() const;
		const std::string				&GetCgiExtension() const;
		const std::string				&GetIndex() const;
		const std::string				&GetRedirect() const;
		const std::vector<std::string>	&GetMethods() const;

		/* Setters */
		void					SetAutoindex(bool autoindex);
		void					SetExactPath(bool exact_path);
		void					SetLocationPath(std::string const &location);
		void					SetRoot(std::string const &root);
		void					SetFullPath(std::string const &s1, std::string const &s2); //root + location
		void					SetCgiPath(std::string const &cgipath);
		void					SetCgiExtension(std::string const &cgiextension);
		void					SetCgi(void);
		void					SetIndex(std::string const &index);
		void					SetRedirect(std::string const &redirect);
		void					SetMethods(std::string const &methods);
		void					SetClientBufferBodySize(size_t body_size);

	//////////////////////////////////////////////
		// size_t&	getBodySize();
		// std::vector<std::string>& getMethods(void);
		// std::string& getRedirect();
		LocationData	&operator=(const LocationData &copy);
};

std::ostream &operator<<(std::ostream& out, const LocationData & lc);

#endif