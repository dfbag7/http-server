#include <algorithm>
#include <cctype>
#include <iomanip>

#include <boost/lexical_cast.hpp>

#include "RequestParser.hpp"
#include "Request.hpp"

// Enable reenter, yield, and fork pseudo-keywords
#include <boost/asio/yield.hpp>

std::string RequestParser::_contentLengthName = "Content-Length";

boost::tribool RequestParser::consume(Request& req, char c)
{

	reenter(this)
	{
		req.method.clear();
		req.uri.clear();
		req.httpVersionMajor = 0;
		req.httpVersionMinor = 0;
		req.headers.clear();
		req.content.clear();

		_contentLength = 0;

		while(isChar(c) && !isCtl(c) && !isTsSpecial(c) && c != ' ')
		{
			req.method.push_back(c);
			yield return boost::indeterminate;
		}

		if(req.method.empty())
			return false;

		if(c != ' ') return false;
		yield return boost::indeterminate;

		while(!isCtl(c) && c != ' ')
		{
			req.uri.push_back(c);
			yield return boost::indeterminate;
		}

		if(c != ' ') return false;
		yield return boost::indeterminate;

		if(c != 'H') return false;
		yield return boost::indeterminate;
		if(c != 'T') return false;
		yield return boost::indeterminate;
		if(c != 'T') return false;
		yield return boost::indeterminate;
		if(c != 'P') return false;
		yield return boost::indeterminate;
		if(c != '/') return false;
		yield return boost::indeterminate;

		if(!isDigit(c)) return false;
		while(isDigit(c))
		{
			req.httpVersionMajor = req.httpVersionMajor * 10 + c - '0';
			yield return boost::indeterminate;
		}

		if(c != '.') return false;
		yield return boost::indeterminate;

		if(!isDigit(c)) return false;
		while(isDigit(c))
		{
			req.httpVersionMinor = req.httpVersionMinor * 10 + c - '0';
			yield return boost::indeterminate;
		}

		if(c != '\r') return false;
		yield return boost::indeterminate;

		if(c != '\n') return false;
		yield return boost::indeterminate;

		while( (isChar(c) && !isCtl(c) && !isTsSpecial(c) && c != '\r') || (c == ' ' || c == '\t') )
		{
			if(c == ' ' || c == '\t')
			{
				if(req.headers.empty())
					return false;

				while(c == ' ' || c == '\t')
				{
					yield return boost::indeterminate;
				}
			}
			else
			{
				req.headers.push_back(Header());

				while( isChar(c) && !isCtl(c) && !isTsSpecial(c) && c != ':')
				{
					req.headers.back().name.push_back(c);
					yield return boost::indeterminate;
				}

				if(c != ':') return false;
				yield return boost::indeterminate;
				if(c != ' ') return false;
				yield return boost::indeterminate;
			}

			while(isChar(c) && !isCtl(c) && c != '\r')
			{
				req.headers.back().value.push_back(c);
				yield return boost::indeterminate;
			}

			if(c != '\r') return false;
			yield return boost::indeterminate;

			if(c != '\n') return false;
			yield return boost::indeterminate;
		}

		if(c != '\r') return false;
		yield return boost::indeterminate;

		if(c != '\n') return false;
		
		//yield return boost::indeterminate;

		for(std::size_t i = 0; i < req.headers.size(); ++i)
		{
			if(headersEqual(req.headers[i].name, _contentLengthName))
			{
				try
				{
					_contentLength = boost::lexical_cast<std::size_t>(req.headers[i].value);
				}
				catch(boost::bad_lexical_cast&)
				{
					return false;
				}
			}
		}

		while(req.content.size() < _contentLength)
		{
			yield return boost::indeterminate;
			req.content.push_back(c);
		}
	}

	return true;
}

// Disable reenter, yield, and fork pseudo-keywords
#include <boost/asio/unyield.hpp>

bool RequestParser::isChar(int c)
{
	return c >= 0 && c < 127;
}

bool RequestParser::isCtl(int c)
{
	return (c >= 0 && c <= 31) || (c == 127);
}

bool RequestParser::isTsSpecial(int c)
{
	switch(c)
	{
		case '(':
		case ')':
		case '<':
		case '>':
		case '@':
		case ',':
		case ';':
		case ':':
		case '\\':
		case '"':
		case '/':
		case '[':
		case ']':
		case '?':
		case '=':
		case '{':
		case '}':
		case ' ':
		case '\t':
			return true;

		default:
			return false;
	}
}

bool RequestParser::isDigit(int c)
{
	return c >= '0' && c <= '9';
}

bool RequestParser::toLowerCompare(char a, char b)
{
	return std::tolower(a) == std::tolower(b);
}

bool RequestParser::headersEqual(const std::string& a, const std::string& b)
{
	if(a.length() != b.length())
		return false;

	return std::equal(a.begin(), a.end(), b.begin(), &RequestParser::toLowerCompare);
}