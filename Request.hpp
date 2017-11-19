#ifndef REQUEST_HTTP
#define REQUEST_HTTP

#include <string>
#include <vector>

#include "Header.hpp"

struct Request
{
	std::string method;
	std::string uri;
	int httpVersionMajor;
	int httpVersionMinor;
	std::vector<Header> headers;
	std::string content;
};

#endif // REQUEST_HTTP