#include <Include_Library.hpp>
//#include "LocationData.hpp"

LocationData::LocationData()
{
	this->_autoindex = false;
	this->_exact_path = false;
	_client_buffer_body_size = 100;
}

LocationData::~LocationData() {}
LocationData::LocationData(const LocationData &cpy) { *this = cpy; }



const std::multimap<std::string, std::string> &LocationData::getCgi() const
{
	return _cgi;
}

LocationData &LocationData::operator=(const LocationData &cpy)
{
	if (this != &cpy)
	{
		this->_location_path = cpy._location_path;
		this->_full_path = cpy._full_path;
		this->_root = cpy._root;
		this->_cgi_extension = cpy._cgi_extension;
		this->_cgi_path = cpy._cgi_path;
		this->_index = cpy._index;
		this->_redirect = cpy._redirect;
		this->_autoindex = cpy._autoindex;
		this->_exact_path = cpy._exact_path;
		this->_methods = cpy._methods;
		this->_client_buffer_body_size = cpy._client_buffer_body_size;
		_autoindex = cpy._autoindex;
	}
	return (*this);
}

const std::string &LocationData::				GetCgiPath() const { return (this->_cgi_path); }
size_t LocationData::							GetClientBufferBodySize() const { return (_client_buffer_body_size); }
const std::string &LocationData::				GetCgiExtension() const { return (this->_cgi_extension); }
const std::string &LocationData::				GetFullPath() const { return (this->_full_path); }
std::string const &LocationData::				GetLocationPath() const { return (this->_location_path); }
const std::string &LocationData::				GetIndex() const { return (this->_index); }
std::string const &LocationData::				GetRoot() const { return (this->_root); }
const std::string &LocationData::				GetRedirect() const { return (this->_redirect); }
const std::vector<std::string> &LocationData::	GetMethods() const { return (this->_methods); }

bool LocationData::IsAutoindex() const { return (this->_autoindex); }
bool LocationData::IsExactPath() const { return (this->_exact_path); }

void LocationData::		SetAutoindex(bool autoindex) { this->_autoindex = autoindex; }
void LocationData::		SetClientBufferBodySize(size_t body_size) { this->_client_buffer_body_size = body_size; }
void LocationData::		SetCgiPath(std::string const &cgipath) { this->_cgi_path = cgipath; }
void LocationData::		SetCgiExtension(std::string const &cgiextension) { this->_cgi_extension = cgiextension; }
void LocationData::		SetCgi(void)
{
	_cgi.insert(std::make_pair(_cgi_extension, _cgi_path));  //TODO: костыль
}
void LocationData::		SetExactPath(bool exact_path) { this->_exact_path = exact_path; }
void LocationData::		SetIndex(std::string const &index) { this->_index = index; }
void LocationData::		SetLocationPath(std::string const &location) { this->_location_path = location; }
void LocationData::		SetRoot(std::string const &root) { this->_root = root; }
void LocationData::		SetRedirect(std::string const &redirect) { this->_redirect = redirect; }

void LocationData::SetFullPath(std::string const &s1, std::string const &s2)
{
	this->_full_path = s1 + s2;
	size_t _find;
	_find = _full_path.find("//");
	if (_find != std::string::npos)
	{
		_full_path.erase(_find, 1); // обрезаем лишний слэш
	}
}

void LocationData::SetMethods(std::string const &methods)
{
	// std::string sentence = "Hello how are you";
	std::istringstream iss(methods);
	// std::vector<std::string> tokens;
	std::copy(std::istream_iterator<std::string>(iss),
			  std::istream_iterator<std::string>(),
			  std::back_inserter<std::vector<std::string> >(_methods));
	// this->_methods.insert();

	// this->_methods = methods;
}

std::ostream &operator<<(std::ostream &out, const LocationData &ld)
{
	out << "Root:         " << (ld.GetRoot().empty() ? "not specified" : ld.GetRoot()) << std::endl;
	out << "AutoIndex:    " << (ld.IsAutoindex() == 1 ? "true" : "false") << std::endl;
	out << "Index:        " << (ld.GetIndex().empty() ? "not specified" : ld.GetIndex()) << std::endl;
	out << "LocationPath: " << ld.GetLocationPath() << std::endl;
	out << "ExactPath:    " << (ld.IsExactPath() == 1 ? "true" : "false") << std::endl;
	out << "FullPath:     " << ld.GetFullPath() << std::endl;
	out << "ClientBSize:  " << ld.GetClientBufferBodySize() << std::endl;
	out << "Redirect:     " << (ld.GetRedirect().empty() ? "not specified" : ld.GetRedirect()) << std::endl;
	out << "CgiExtension: " << (ld.GetCgiExtension().empty() ? "not specified" : ld.GetCgiExtension()) << std::endl;
	out << "CgiPath:      " << (ld.GetCgiPath().empty() ? "not specified" : ld.GetCgiPath()) << std::endl;
	out << std::endl;
	return (out);
}