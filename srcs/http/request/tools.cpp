
#include "tools.hpp"

std::size_t	skipWhiteSpaces(std::string const &str, std::size_t start)
{
	if (start >= str.length())
		return str.length();
	while (start < str.length() and isCharWhiteSpace(str[start]))
		start++;
	return start;
}

bool	isCharWhiteSpace(unsigned char c)
{
	return std::isspace(c);
}