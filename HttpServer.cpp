#include "HttpServer.hpp"
#include "Request.hpp"
#include "Reply.hpp"

HttpServer::HttpServer(boost::asio::io_service& ioService,
	const std::string& address, const std::string& port,
	boost::function<void(const Request&, Reply&)> requestHandler)
		: _requestHandler(requestHandler)
{	
	tcp::resolver resolver(ioService);
	tcp::resolver::query query(address, port);

	tcp::resolver::iterator itr = resolver.resolve(query);

	_acceptor.reset(new tcp::acceptor(ioService, *itr));
}

// Enable the pseudo-keywords reenter, yield and fork
#include <boost/asio/yield.hpp>

void HttpServer::operator()(boost::system::error_code errCode, std::size_t length)
{
	if( !errCode )
	{
		reenter(this)
		{
			do
			{
				_socket.reset(new tcp::socket(_acceptor->get_io_service()));

				yield _acceptor->async_accept(*_socket, *this);

				fork HttpServer(*this)();
			} while(is_parent());

			_buffer.reset(new boost::array<char, 8192>);
			_request.reset(new Request());

			do
			{
				yield _socket->async_read_some(boost::asio::buffer(*_buffer), *this);

				boost::tie(_validRequest, boost::tuples::ignore)
					= _requestParser.parse(*_request, _buffer->data(), _buffer->data() + length);
			} while(boost::indeterminate(_validRequest));

			_reply.reset(new Reply());

			if(_validRequest)
			{
				_requestHandler(*_request, *_reply);
			}
			else
			{
				*_reply = Reply::stockReply(Reply::BadRequest);
			}

			yield boost::asio::async_write(*_socket, _reply->toBuffers(), *this);

			_socket->shutdown(tcp::socket::shutdown_both, errCode);
		}
	}
}

#include <boost/asio/unyield.hpp>
