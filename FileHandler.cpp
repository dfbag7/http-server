#include "FileHandler.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>

#include "MimeTypes.hpp"
#include "Reply.hpp"
#include "Request.hpp"

FileHandler::FileHandler(const std::string& docRoot)
		: _docRoot(docRoot)
{
	//
}

void FileHandler::operator()(const Request& req, Reply& rep)
{
	std::string requestPath;
	if( !decodeUrl(req.uri, requestPath) )
	{
		rep = Reply::stockReply(Reply::BadRequest);
		return;
	}

	if( requestPath.empty() || requestPath[0] != '/' || requestPath.find("..") != std::string::npos )
	{
		rep = Reply::stockReply(Reply::BadRequest);
		return;
	}

	if( requestPath[requestPath.size() - 1] == '/')
	{
		requestPath += "index.html";
	}

	std::size_t lastSlashPos = requestPath.find_last_of("/");
	std::size_t lastDotPos = requestPath.find_last_of(".");
	std::string extension;

	if( lastDotPos != std::string::npos && lastDotPos > lastSlashPos )
	{
		extension = requestPath.substr(lastDotPos + 1);
	}

	std::string fullPath = _docRoot + requestPath;
	std::ifstream is(fullPath.c_str(), std::ios::in | std::ios::binary);
	if( !is )
	{
		rep = Reply::stockReply(Reply::NotFound);
		return;
	}

	rep.status = Reply::Ok;
	char buf[512];
	while( is.read(buf, sizeof(buf)).gcount() > 0 )
		rep.content.append(buf, is.gcount());

	rep.headers.resize(2);
	rep.headers[0].name = "Content-Length";
	rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
	rep.headers[1].name = "Content-Type";
	rep.headers[1].value = MimeTypes::extensionToType(extension);
}

bool FileHandler::decodeUrl(const std::string& in, std::string& out)
{
	out.clear();
	out.reserve(in.size());

	for(std::size_t i = 0; i < in.size(); ++i)
	{
		if( in[i] == '%' )
		{
			if( i + 3 <= in.size() )
			{
				int value = 0;
				std::istringstream is(in.substr(i + 1, 2));
				if( is >> std::hex >> value )
				{
					out += static_cast<char>(value);
					i += 2;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else if(in[i] == '+')
		{
			out += ' ';
		}
		else
		{
			out += in[i];
		}
	}

	return true;
}
