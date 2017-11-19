#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include <string>

struct Reply;
struct Request;

class FileHandler
{
public:

	explicit FileHandler(const std::string& docRoot);

	void operator()(const Request& req, Reply& rep);

private:
	std::string _docRoot;
	static bool decodeUrl(const std::string& in, std::string& out);
};

#endif // FILE_HANDLER_HPP