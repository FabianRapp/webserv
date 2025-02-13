/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   Request.cpp										:+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: adrherna <adrianhdt.2001@gmail.com>		+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2025/01/13 20:28:01 by adrherna		  #+#	#+#			 */
/*   Updated: 2025/01/22 14:02:45 by adrherna		 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#include "../includes/Request.hpp"
#include <colors.h>
#include <logging.hpp>

Request::Request()
{
	_finished = false;
	_areHeadersParsed = false;
	_type = MethodType::INVALID;
	_uri = "";
	_version = "";
	_body = "";
	_startBodyIdx = 0;
	set_status_code(200);
}

Request::Request(const Request& old):
	_finished(old._finished),
	_areHeadersParsed(old._areHeadersParsed),
	_type(old._type),
	_uri(old._uri),
	_version(old._version),
	_headers(old._headers),
	_body(old._body),
	_startBodyIdx(old._startBodyIdx),
	_bodyTokens(old._bodyTokens),
	_response_str(old._response_str),
	_status_code(old._status_code),
	additional_response_headers(old.additional_response_headers)
{
}

Request::Request(Request&& old):
	_finished(std::move(old._finished)),
	_areHeadersParsed(std::move(old._areHeadersParsed)),
	_type(std::move(old._type)),
	_uri(std::move(old._uri)),
	_version(std::move(old._version)),
	_headers(std::move(old._headers)),
	_body(std::move(old._body)),
	_startBodyIdx(std::move(old._startBodyIdx)),
	_bodyTokens(std::move(old._bodyTokens)),
	_response_str(std::move(old._response_str)),
	_status_code(std::move(old._status_code)),
	additional_response_headers(std::move(old.additional_response_headers))
{
}

Request	Request::operator=(const Request& old) {
	if (this == &old) {
		return (*this);
	}
	_finished = old._finished;
	_areHeadersParsed = old._areHeadersParsed;
	_type = old._type;
	_uri = old._uri;
	_version = old._version;
	_headers = old._headers;
	_body = old._body;
	_startBodyIdx = old._startBodyIdx;
	_bodyTokens = old._bodyTokens;
	_response_str = old._response_str;
	_status_code = old._status_code;
	additional_response_headers = old.additional_response_headers;
	return (*this);
}

Request	Request::operator=(Request&& old) {
	if (this == &old) {
		return (*this);
	}
	_finished = std::move(old._finished);
	_areHeadersParsed = std::move(old._areHeadersParsed);
	_type = std::move(old._type);
	_uri = std::move(old._uri);
	_version = std::move(old._version);
	_headers = std::move(old._headers);
	_body = std::move(old._body);
	_startBodyIdx = std::move(old._startBodyIdx);
	_bodyTokens = std::move(old._bodyTokens);
	_response_str = std::move(old._response_str);
	_status_code = std::move(old._status_code);
	additional_response_headers = std::move(old.additional_response_headers);

	return (*this);
}

void Request::set_status_code(int code) {
	static const std::unordered_map<int, std::string> codes = {
		{100, "Continue"},
		{101, "Switching Protocols"},
		{102, "Processing"},
		{103, "Early Hints"},

		{200, "OK"},
		{201, "Created"},
		{202, "Accepted"},
		{203, "Non-Authoritative Information"},
		{204, "No Content"},
		{205, "Reset Content"},
		{206, "Partial Content"},
		{207, "Multi-Status"},
		{208, "Already Reported"},
		{226, "IM Used"},

		{300, "Multiple Choices"},
		{301, "Moved Permanently"},
		{302, "Found"},
		{303, "See Other"},
		{304, "Not Modified"},
		{305, "Use Proxy"},
		{307, "Temporary Redirect"},
		{308, "Permanent Redirect"},

		{400, "Bad Request"},
		{401, "Unauthorized"},
		{402, "Payment Required"},
		{403, "Forbidden"},
		{404, "Not Found"},
		{405, "Method Not Allowed"},
		{406, "Not Acceptable"},
		{407, "Proxy Authentication Required"},
		{408, "Request Timeout"},
		{409, "Conflict"},
		{410, "Gone"},
		{411, "Length Required"},
		{412, "Precondition Failed"},
		{413, "Payload Too Large"},
		{414, "URI Too Long"},
		{415, "Unsupported Media Type"},
		{416, "Range Not Satisfiable"},
		{417, "Expectation Failed"},
		{418, "I'm a teapot"},
		{421, "Misdirected Request"},
		{422, "Unprocessable Entity"},
		{423, "Locked"},
		{424, "Failed Dependency"},
		{425, "Too Early"},
		{426, "Upgrade Required"},
		{428, "Precondition Required"},
		{429, "Too Many Requests"},
		{431, "Request Header Fields Too Large"},
		{451, "Unavailable For Legal Reasons"},

		{500, "Internal Server Error"},
		{501, "Not Implemented"},
		{502, "Bad Gateway"},
		{503, "Service Unavailable"},
		{504, "Gateway Timeout"},
		{505, "HTTP Version Not Supported"},
		{506, "Variant Also Negotiates"},
		{507, "Insufficient Storage"},
		{508, "Loop Detected"},
		{510, "Not Extended"},
		{511, "Network Authentication Required"}
	};

	if (codes.find(code) == codes.end()) {
		LOG(FT_ANSI_RED "unsupported status code: " << code << FT_ANSI_RESET << std::endl);
		code = 500;
	}
	_status_code.first = code;
	_status_code.second = codes.at(code);
}
