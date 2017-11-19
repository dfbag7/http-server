#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <boost/array.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "RequestParser.hpp"

struct Request;
struct Reply;

class HttpServer : boost::asio::coroutine
{
public:
	explicit HttpServer(boost::asio::io_service& ioService,
		const std::string& address, const std::string& port,
		boost::function<void(const Request&, Reply&)> requestHandler);

	void operator()(boost::system::error_code errCode = boost::system::error_code(),
		std::size_t length = 0);

private:
	typedef boost::asio::ip::tcp tcp;

	boost::function<void(const Request&, Reply&)> _requestHandler;

	boost::shared_ptr<tcp::acceptor> _acceptor;
	boost::shared_ptr<tcp::socket> _socket;
	boost::shared_ptr<boost::array<char, 8192> > _buffer;
	boost::shared_ptr<Request> _request;
	boost::tribool _validRequest;
	RequestParser _requestParser;
	boost::shared_ptr<Reply> _reply;
};

#endif // HTTP_SERVER_HPP