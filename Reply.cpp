#include "Reply.hpp"

#include <string>
#include <boost/lexical_cast.hpp>

namespace StatusStrings
{
	const std::string Ok = "HTTP/1.0 200 OK\r\n";
	const std::string Created = "HTTP/1.0 201 Created\r\n";
	const std::string Accepted = "HTTP/1.0 202 Accepted\r\n";
	const std::string NoContent = "HTTP/1.0 204 No Content\r\n";
	const std::string MultipleChoices = "HTTP/1.0 300 Multiple Choices\r\n";
	const std::string MovedPermanently = "HTTP/1.0 301 Moved Permanently\r\n";
	const std::string MovedTemporarily = "HTTP/1.0 302 Moved Temporarily\r\n";
	const std::string NotModified = "HTTP/1.0 304 Not Modified\r\n";
	const std::string BadRequest = "HTTP/1.0 400 Bad Request\r\n";
	const std::string Unauthorized = "HTTP/1.0 401 Unauthorized\r\n";
	const std::string Forbidden = "HTTP/1.0 403 Forbidden\r\n";
	const std::string NotFound = "HTTP/1.0 404 Not Found\r\n";
	const std::string InternalServerError = "HTTP/1.0 500 Internal Server Error\r\n";
	const std::string NotImplemented = "HTTP/1.0 501 Not Implemented\r\n";
	const std::string BadGateway = "HTTP/1.0 502 Bad Gateway\r\n";
	const std::string ServiceUnavailable = "HTTP/1.0 503 Service Unavailable\r\n";

	boost::asio::const_buffer toBuffer(Reply::StatusType status)
	{
		switch(status)
		{
			case Reply::Ok:                  return boost::asio::buffer(Ok);
			case Reply::Created:             return boost::asio::buffer(Created);
			case Reply::Accepted:            return boost::asio::buffer(Accepted);
			case Reply::NoContent:           return boost::asio::buffer(NoContent);
			case Reply::MultipleChoices:     return boost::asio::buffer(MultipleChoices);
			case Reply::MovedPermanently:    return boost::asio::buffer(MovedPermanently);
			case Reply::MovedTemporarily:    return boost::asio::buffer(MovedTemporarily);
			case Reply::NotModified:         return boost::asio::buffer(NotModified);
            case Reply::BadRequest:          return boost::asio::buffer(BadRequest);
            case Reply::Unauthorized:        return boost::asio::buffer(Unauthorized);
            case Reply::Forbidden:           return boost::asio::buffer(Forbidden);
            case Reply::NotFound:            return boost::asio::buffer(NotFound);
            case Reply::InternalServerError: return boost::asio::buffer(InternalServerError);
            case Reply::NotImplemented:      return boost::asio::buffer(NotImplemented);
            case Reply::BadGateway:          return boost::asio::buffer(BadGateway);
            case Reply::ServiceUnavailable:  return boost::asio::buffer(ServiceUnavailable);

            default:                         return boost::asio::buffer(InternalServerError);
		}
	}
} // namespace StatusStrings

namespace MiscStrings 
{

const char nameValueSeparator[] = { ':', ' '};
const char crlf[] = { '\r', '\n' };

} // namespace MiscStrings

std::vector<boost::asio::const_buffer> Reply::toBuffers()
{
	std::vector<boost::asio::const_buffer> buffers;
	buffers.push_back(StatusStrings::toBuffer(status));

	for(std::size_t i = 0; i < headers.size(); ++i)
	{
		Header& h = headers[i];
		buffers.push_back(boost::asio::buffer(h.name));
		buffers.push_back(boost::asio::buffer(MiscStrings::nameValueSeparator));
		buffers.push_back(boost::asio::buffer(h.value));
		buffers.push_back(boost::asio::buffer(MiscStrings::crlf));
	}

	buffers.push_back(boost::asio::buffer(MiscStrings::crlf));
	buffers.push_back(boost::asio::buffer(content));

	return buffers;
}

namespace StockReplies
{
	const char Ok[] = "";

	const char Created[] = "<html>"
	                       "<head><title>Created</title></head>"
	                       "<body><h1>201 Created</h1></body>"
	                       "</html>";

	const char Accepted[] = "<html>"
	                        "<head><title>Accepted</title></head>"
	                        "<body><h1>202 Accepted</h1></body>"
	                        "</html>";


	const char NoContent[] = "<html>"
	                        "<head><title>No Content</title></head>"
	                        "<body><h1>204 No Content</h1></body>"
	                        "</html>";

	const char MultipleChoices[] = "<html>"
	                        "<head><title>Multiple Choices</title></head>"
	                        "<body><h1>300 Multiple Choices</h1></body>"
	                        "</html>";

	const char MovedPermanently[] = "<html>"
	                        "<head><title>Moved Permanently</title></head>"
	                        "<body><h1>301 Moved Permanently</h1></body>"
	                        "</html>";

	const char MovedTemporarily[] = "<html>"
	                        "<head><title>Moved Temporarily</title></head>"
	                        "<body><h1>302 Moved Temporarily</h1></body>"
	                        "</html>";

	const char NotModified[] = "<html>"
	                        "<head><title>Not Modified</title></head>"
	                        "<body><h1>304 Not Modified</h1></body>"
	                        "</html>";

	const char BadRequest[] = "<html>"
	                        "<head><title>Bad Request</title></head>"
	                        "<body><h1>400 Bad Request</h1></body>"
	                        "</html>";

	const char Unauthorized[] = "<html>"
	                        "<head><title>Unauthorized</title></head>"
	                        "<body><h1>401 Unauthorized</h1></body>"
	                        "</html>";

	const char Forbidden[] = "<html>"
	                        "<head><title>Forbidden</title></head>"
	                        "<body><h1>403 Forbidden</h1></body>"
	                        "</html>";

	const char NotFound[] = "<html>"
	                        "<head><title>Not Found</title></head>"
	                        "<body><h1>404 Not Found</h1></body>"
	                        "</html>";

	const char InternalServerError[] = "<html>"
	                        "<head><title>Internal Server Error</title></head>"
	                        "<body><h1>500 Internal Server Error</h1></body>"
	                        "</html>";

	const char NotImplemented[] = "<html>"
	                        "<head><title>Not Implemented</title></head>"
	                        "<body><h1><501 Not Implemented/h1></body>"
	                        "</html>";

	const char BadGateway[] = "<html>"
	                        "<head><title>Bad Gateway</title></head>"
	                        "<body><h1>502 Bad Gateway</h1></body>"
	                        "</html>";

	const char ServiceUnavailable[] = "<html>"
	                        "<head><title>Service Unavailable</title></head>"
	                        "<body><h1>503 Service Unavailable</h1></body>"
	                        "</html>";

std::string toString(Reply::StatusType status)
{
		switch(status)
		{
			case Reply::Ok:                  return Ok;
			case Reply::Created:             return Created;
			case Reply::Accepted:            return Accepted;
			case Reply::NoContent:           return NoContent;
			case Reply::MultipleChoices:     return MultipleChoices;
			case Reply::MovedPermanently:    return MovedPermanently;
			case Reply::MovedTemporarily:    return MovedTemporarily;
			case Reply::NotModified:         return NotModified;
            case Reply::BadRequest:          return BadRequest;
            case Reply::Unauthorized:        return Unauthorized;
            case Reply::Forbidden:           return Forbidden;
            case Reply::NotFound:            return NotFound;
            case Reply::InternalServerError: return InternalServerError;
            case Reply::NotImplemented:      return NotImplemented;
            case Reply::BadGateway:          return BadGateway;
            case Reply::ServiceUnavailable:  return ServiceUnavailable;

            default:                         return InternalServerError;
		}
}

} // namespace StockReplies	   

Reply Reply::stockReply(Reply::StatusType status)
{
	Reply result;
	result.status = status;
	result.content = StockReplies::toString(status);
	result.headers.resize(2);
	result.headers[0].name = "Content-Length";
	result.headers[0].value = boost::lexical_cast<std::string>(result.content.size());
	result.headers[1].name = "Content-Type";
	result.headers[1].value = "text/html";

	return result;
}                    
