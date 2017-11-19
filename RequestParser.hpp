#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <string>
#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/asio/coroutine.hpp>

struct Request;

class RequestParser: boost::asio::coroutine
{
public:

	template <class InputIterator>
	boost::tuple<boost::tribool, InputIterator> parse(Request& req, InputIterator begin, InputIterator end)
	{
		while(begin != end)
		{
			boost::tribool result = consume(req, *begin);
			++begin;
			if(result || !result)
			{
				return boost::make_tuple(result, begin);
			}
		}

		boost::tribool result = boost::indeterminate;

		return boost::make_tuple(result, begin);
	}

private:

	static std::string _contentLengthName;

	std::size_t _contentLength;

	boost::tribool consume(Request& req, char input);
	bool headersEqual(const std::string& a, const std::string& b);

	static bool isChar(int c);
	static bool isCtl(int c);
	static bool isTsSpecial(int c);
	static bool isDigit(int c);
	static bool toLowerCompare(char a, char b);
};

#endif // REQUEST_PARSER_HPP