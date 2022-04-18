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

	std::string 			_location_path;
	// std::string 			_path;
	std::string 			_root;
	std::string 			_full_path;
	std::string				_cgi_path;
	std::string				_cgi_extension;
	std::string 			_index;
	std::string 			_redirect;
	std::vector<std::string>	_methods;

	/*  
		Сравниваем размер тела из запроса для ответа.
		Если тело в запросе request больше, чем 100 символов, то в ответе возвращается ошибка - посмотреть номер в http
	*/
	size_t					_client_buffer_body_size; 
	bool					_autoindex;

public:
	LocationData();
	~LocationData();
	LocationData(const LocationData &copy);

	/* Getters */
	bool							getAutoindex() const;
	std::string const				&getLocationPath() const;
	size_t							getClientBufferBodySize() const;
	const std::string				&getRoot() const;
	const std::string				&getFullPath() const;
	const std::string				&getCgiPath() const;
	const std::string				&getCgiExtension() const;
	const std::string				&getIndex() const;
	const std::string				&getRedirect() const;
	const std::vector<std::string>	&getMethods() const;

	/* Setters */
	void					setAutoindex(bool autoindex);
	void					setLocationPath(std::string const &location);
	void					setRoot(std::string const &root);
	void					setFullPath(std::string const &s1, std::string const &s2); //root + location
	void					setCgiPath(std::string const &cgipath);
	void					setCgiExtension(std::string const &cgiextension);
	void					setIndex(std::string const &index);
	void					setRedirect(std::string const &redirect);
	void					setMethods(std::string const &methods);
	void					setClientBufferBodySize(size_t body_size);

//////////////////////////////////////////////
	// size_t&	getBodySize();
	// std::vector<std::string>& getMethods(void);
	// std::string& getRedirect();
	LocationData	&operator=(const LocationData &copy);
};

std::ostream &operator<<(std::ostream& out, const LocationData & lc);

#endif