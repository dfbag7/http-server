#ifndef REPLY_HPP
#define REPLY_HPP

#include <string>
#include <vector>
#include <boost/asio.hpp>

#include "Header.hpp"

struct Reply
{
	enum StatusType
	{
		Ok = 200,
		Created = 201,
		Accepted = 202,
		NoContent = 204,
		MultipleChoices = 300,
		MovedPermanently = 301,
		MovedTemporarily = 302,
		NotModified = 304,
		BadRequest = 400,
		Unauthorized = 401,
		Forbidden = 403,
		NotFound = 404,
		InternalServerError = 500,
		NotImplemented = 501,
		BadGateway = 502,
		ServiceUnavailable = 503,
	} status;

	std::vector<Header> headers;
	std::string content;
	std::vector<boost::asio::const_buffer> toBuffers();

	static Reply stockReply(StatusType status);
};

#endif // REPLY_HPP