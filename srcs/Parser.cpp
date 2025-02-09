/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrherna <adrianhdt.2001@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 13:43:05 by adrherna          #+#    #+#             */
/*   Updated: 2025/02/05 12:31:17 by adrherna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/parser/Parser.hpp"
#include <cstddef>
#include <string>
#include <vector>

// 1. No Content-Length or Transfer-Encoding in HTTP/1.1
// Connection Closure:
// If neither Content-Length nor Transfer-Encoding is specified, the end of the body is signaled by the server closing the connection.
// This is a fallback mechanism in HTTP/1.1 but is considered bad practice and is rarely used.

Parser::Parser(std::string& input, const std::vector<ServerConfigFile>& configs):
	_input(input),
	_max_request_body_size(-1),
	_server_configs(configs),
	_config_index(-1),
	_location_index(-1)
{
}

void	Parser::_select_server_config(void) {
	FT_ASSERT(_request._areHeadersParsed);

	if (_request._headers.find(HeaderType::HOST) == _request._headers.end()) {
		_config_index = 0;
		return ;
	}

	std::string	to_match = _request._headers[HeaderType::HOST];

	std::transform(to_match.begin(), to_match.end(), to_match.begin(),
		[](unsigned char c) { return (std::tolower(c));});



	for (size_t i = 0; i < _server_configs.size(); i++) {
		const auto& config = _server_configs[i];
		const std::vector<std::string>&	names = config.getServerNames();
		for (const auto& name : names) {
			std::string	postfix = ":" + std::to_string(config.getPort());
			std::string	prefix = "www.";
			if (name == to_match
				|| to_match == prefix + name + postfix
				|| to_match == prefix + name
				|| to_match == name + postfix)
			{
				std::cout << "found!\n";
				_config_index = static_cast<int>(i);
				return ;
			}
		}
	}
	_config_index = 0;
}

void	Parser::_select_location_config(void) {
	FT_ASSERT(_config_index >= 0);

	const std::vector<LocationConfigFile>	&locationsFiles =
		_server_configs[static_cast<size_t>(_config_index)].getLocations();

	size_t				longest_match = 0;
	for (size_t i = 0; i < locationsFiles.size(); i++) {
		const LocationConfigFile& locationFile = locationsFiles[i];
		size_t	loc_path_len = locationFile.getPath().length();
		if (loc_path_len > longest_match
			&& !strncmp(_request._uri.c_str(), locationFile.getPath().c_str(), loc_path_len)
			&& (_request._uri.length() == loc_path_len || _request._uri[loc_path_len] == '/'))
		{
			longest_match = loc_path_len;
			_location_index = static_cast<int>(i);
		}
	}
}

void	Parser::_select_config(void) {
	_select_server_config();
	_select_location_config();
}

// only callabled once _select_config(void) was used
const ServerConfigFile&	Parser::get_config(void) const {
	FT_ASSERT(_config_index >= 0 && "config has not been set");
	return (_server_configs[static_cast<size_t>(_config_index)]);
}

// only callabled once _select_config(void) was used
const LocationConfigFile&	Parser::get_location_config(void) const {
	if (_location_index == -1) {
		return (get_config().getDefaultLocation());
	}
	const std::vector<LocationConfigFile>	&locations_files = get_config().getLocations();
	return (locations_files[static_cast<size_t>(_location_index)]);
}

int sizeLineToInt(const std::string& hexStr) {
	int value = 0;
	std::stringstream ss;

	ss << std::hex << hexStr;
	ss >> value;

	return value;
}

std::string	cleanBody(const std::string& input) {
	std::string	cleanBody;
	size_t		start;

	start = input.find("\r\n\r\n");
	if (start == std::string::npos) {
		std::cout << "could not find end of headers" << std::endl;
		// handle error logic
		return ("");
	}

	cleanBody = input.substr(start + 4);

	return (cleanBody);
}
/*
void printRequestArray(const RequestArray& arr) {
	std::cout << "string arr:\n";
	for (size_t i = 0; i < arr.size(); ++i) {
		std::cout << "Line " << i + 1 << ": ";
		for (const auto& word : arr[i]) {
			std::cout << word << " ";
		}
		std::cout << std::endl;
	}
}
*/
/*
//splits a string into the individual words and puts it inside a vector. Like in a char **.
std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = str.find(delimiter);

	while (end != std::string::npos) {
		std::string token = str.substr(start, end - start);
		if (end != start) {
			tokens.push_back(token);
		}
		if (str.find(delimiter, start) != std::string::npos) {
			tokens.push_back(delimiter);
		}
		start = end + delimiter.length();
		end = str.find(delimiter, start);
	}

	return tokens;
}
*/

/*
// Function to split the input string into arrays (lines and words) and stop at stopDl
// Dl = delimiter
RequestArray splitIntoArrays(const String& input, const String& lineDl, const String& stopDl) {
	String str;
	RequestArray result;
	size_t start = 0, end = 0;

	std::cout << "input: " << input << "\n";
	end = input.find(stopDl);
	if (end == std::string::npos) {
		std::cout << "could not find stopDl" << std::endl;
		return result;
	}

	str = input.substr(start, end);

	while ((end = str.find(lineDl, start)) != std::string::npos) {
		String line = str.substr(start, end - start);
		Line lineV = split(line, " ");
		result.push_back(lineV);

		start = end + lineDl.length();
	}

	if (start < str.length()) {
		String lastLine = str.substr(start);
		Line lastLineV = split(lastLine, " ");
		result.push_back(lastLineV);
	}

	return result;
}
*/


//Note, I think 405 not allwed might be easier to implement in Response,
// a this point we dont have yet parsed the allowed methods
// either we move all that logic here of throw that specific error later
int Parser::getErrorCode() {


	// if (_request._type == MethodType::INVALID)
	// {
		// Either 405 not allowed or 501 not implemented
		// return 501;
	// }


	// Not Found (404)
	// The requested resource does not exist on the server
	// if (isPathExisting(_request._uri))
	// {
	// 	// 404 not found
	// return 404;
	// }


	// Forbidden (403)
	// The user does not have permission to access the requested resource
	// if (isPathAllowed(_request._uri))
	// {
	// 	// 403 forbidden
	// 	return 403;
	// }


	// Payload Too Large (413)
	// The request body exceeds the allowed size limit
	// if (isRequestTooLarge())
	// {
	// 	return 413;
	// }


	// (415) Unsupported Media Type
	// This happens when the Content-Type of the request is not supported by the server.
	// if (isMediaTypeAllowed())
	// {
		// return 415
	// }


	// Bad Request (400)
	// Malformed syntax (e.g., missing brackets in JSON payload)
	// Invalid query parameters (e.g., incorrect data type)
	// Missing required headers or parameters

	// in the next if block we can make more checks that will also result in a 400
	// if (isInvalidHeader(_request._headers) || ...)
	// {
	// 	// 400 Bad request probably
	// 	return 400;
	// }


	// We could implement the next errors for general errors in the parsing:
	// Unprocessable Entity (422)
	// The server understands the request, but the content is invalid (e.g., failing validation rules)

	// Internal Server Error (500)
	// The server encounters an unexpected condition (often due to a bug or misconfiguration)
	return 0;
}

void Parser::set_max_request_body_size(int max_request_body_size) {
	_max_request_body_size = max_request_body_size;
}

void Parser::setRequestMethod(const std::string& method) {
	if (method == "GET") {
		_request._type = MethodType::GET;
	} else if (method == "POST") {
		_request._type = MethodType::POST;
	} else if (method == "PUT") {
		_request._type = MethodType::PUT;
	} else if (method == "DELETE") {
		_request._type = MethodType::DELETE;
	} else if (method == "PUT") {
		_request._type = MethodType::PUT;
	} else {
		_request._type = MethodType::INVALID;
	}
}

void	Parser::setUri(const std::string& uri) {
	_request._uri = uri;
}

void	Parser::setVersion(const std::string& version) {
	_request._version = version;
}

HeaderType	setType(const std::string& str) {
	std::pair<const std::string, HeaderType>	matches[] = {
		{"Accept:",                  HeaderType::ACCEPT},
		{"Content-Length:",          HeaderType::CONTENT_LENGTH},
		{"Accept-Charset:",          HeaderType::ACCEPT_CHARSET},
		{"Accept-Encoding:",         HeaderType::ACCEPT_ENCODING},
		{"Accept-Language:",         HeaderType::ACCEPT_LANGUAGE},
		{"Authorization:",           HeaderType::AUTHORIZATION},
		{"Expect:",                  HeaderType::EXPECT},
		{"From:",                    HeaderType::FROM},
		{"Host:",                    HeaderType::HOST},
		{"If-Match:",                HeaderType::IF_MATCH},
		{"If-Modified-Since:",       HeaderType::IF_MODIFIED_SINCE},
		{"If-None-Match:",           HeaderType::IF_NONE_MATCH},
		{"If-Range:",                HeaderType::IF_RANGE},
		{"If-Unmodified-Since:",     HeaderType::IF_UNMODIFIED_SINCE},
		{"Range:",                   HeaderType::RANGE},
		{"Referer:",                 HeaderType::REFERER},
		{"TE:",                      HeaderType::TE},
		{"User-Agent:",              HeaderType::USER_AGENT},

		{"Cache-Control:",           HeaderType::CACHE_CONTROL},
		{"Connection:",              HeaderType::CONNECTION},
		{"Date:",                    HeaderType::DATE},
		{"Pragma:",                  HeaderType::PRAGMA},
		{"Trailer:",                 HeaderType::TRAILER},
		{"Transfer-Encoding:",       HeaderType::TRANSFER_ENCODING},
		{"Upgrade:",                 HeaderType::UPGRADE},
		{"Via:",                     HeaderType::VIA},
		{"Warning:",                 HeaderType::WARNING},

		{"Cookie:",                  HeaderType::COOKIE},
		{"Set-Cookie:",              HeaderType::SET_COOKIE},
	};

	for (const auto& type : matches) {
		if (type.first == str) {
			return (type.second);
		}
	}
	return (HeaderType::INVALID);
}

void Parser::insertHeader(const std::string& key, const std::string& value) {
	HeaderType keyType = setType(key);
	_request._headers.insert({keyType, value});

	// std::cout << "Header:" << std::endl;
	// std::cout << "key: " << key << " value: " << value << std::endl;
}

void	Parser::parse_first_line(const RequestArray& array) {
	if (array[0].size() > 3) {
		_request.set_status_code(400);
		_request._finished = true;
	} else if (array[0].size() != 3) {
		//todo:
		//first line is incomplete
	} else {
		setRequestMethod(array[0][0]);
		setUri(array[0][1]);
		setVersion(array[0][2]);
	}
}

void	Parser::parse_headers(const RequestArray& array) {
	size_t i = 1;

	while (i < array.size())
	{
		if (array[i].size() != 2)
		{
			_request.set_status_code(400);
			_request._finished = true;
			return ;
		}
		insertHeader(array[i][0], array[i][1]);
		i++;
	}
	std::cout << "Headers were parsed\n" << std::endl;
	_request._areHeadersParsed = true;
}

void	Parser::parser_unchunked(std::string& input) {

	unsigned long bytesToRead = 0;
	auto it = _request._headers.find(HeaderType::CONTENT_LENGTH);

	std::cout << "parsing unchunked body\n";
	if (it != _request._headers.end()) {
		bytesToRead = std::stoul(it->second);
	} else {
		std::cout << "Content-Length header not found!" << std::endl;
	}

	// what happens if input is smaller than ConLen ? does that mean that we havent finished reading the full body ?
	// this is working bc we dont actually assing the input to the body until the full body is there.
	if (input.size() < bytesToRead) {
		std::cout << "Input is smaller than Content-Length!" << std::endl;
		return;
	}

	for (unsigned int i = 0; i < bytesToRead; ++i) {
		char currentChar = input[i];
		std::cout << currentChar;
		_request._body.push_back(currentChar);
	}

	_request._finished = true;
	std::cout << "\nFinished reading unchunked body." << std::endl;
}

void printStringVector(const std::vector<std::string>& vec) {
	for (const auto& str : vec) {
		if (str == "\r\n")
			std::cout << "|" << "\\r\\n" << "|" << std::endl;
		else
			std::cout << "|" << str << "|" << std::endl;
	}
}

bool isChunkedFinished(const std::vector<std::string>& bodyVector) {
	if (bodyVector.size() < 3) {
		std::cout << "Error: The body is not long enough, possibly malformed." << std::endl;
		return false;
	}
	size_t start = bodyVector.size() - 3;

	std::cout << "Last 3 elements: " << bodyVector[start] << ", " << bodyVector[start + 1] << ", " << bodyVector[start + 2] << std::endl;

	if (bodyVector[start] != "0" || bodyVector[start + 1] != "\r\n" || bodyVector[start + 2] != "\r\n") {
		std::cout << "Error: Body does not end as expected or is incomplete." << std::endl;
		return false;
	}

	return true;
}

void Parser::addTokens(const std::string& str, const std::string& delimiter) {
	size_t end = str.find(delimiter, _request._startBodyIdx);

	while (end != std::string::npos) {
		std::string token = str.substr(_request._startBodyIdx, end - _request._startBodyIdx);
		if (end != _request._startBodyIdx) {
			_request._bodyTokens.push_back(token);
			// std::cout << "Added token |" << token << "|\n";
		}

		_request._bodyTokens.push_back(delimiter);
		// std::cout << "Added |delimiter|\n";

		_request._startBodyIdx = end + delimiter.length();
		end = str.find(delimiter, _request._startBodyIdx);
	}

	if (_request._startBodyIdx < str.length()) {
		std::string token = str.substr(_request._startBodyIdx);
		_request._bodyTokens.push_back(token);
		// std::cout << "Added final token |" << token << "|\n";
		_request._startBodyIdx = str.length();
	}
}

void	Parser::checkForChunks(std::vector<std::string>& bodyVector) {
	size_t		chunkSize;
	std::string	chunk;

	// std::cout << "ENTERED FOR CHECKS" << std::endl;

	if (bodyVector.size() < 4)
	{
		std::cout << "check for chunks returned, size is to small to contain a chunk" << std::endl;
		return ;
	}

	if (bodyVector[1] != "\r\n" && bodyVector[3] != "\r\n")
	{
		std::cout << "tokens are not terminated by end sequence" << std::endl;
		return;
	} else {

		// chunkSize = std::stoi(bodyVector[0]);
		// std::cout << "Chunk size: " << chunkSize << std::endl;
		chunk = bodyVector[2];
	}
	// std::cout << "Chunk: " << chunk << std::endl;

	// if (chunkSize != chunk.length())
	// {
	// 	std::cout << "Chunk and chunkSize are not matching" << std::endl;
	// 	return;
	// }
	_request._body += chunk;
	// std::cout << "chunk added: |" << chunk << "|" <<std::endl;

	// removing elements bc they where parsed to the body
	bodyVector.erase(bodyVector.begin(), bodyVector.begin() + 4);

	if ((bodyVector.size() - 4) < 4)
	{
		// TEST this with a send that contains more than one chunk
		std::cout << "Recursin for checkForChunks called\n";
		checkForChunks(bodyVector);
	}
}

// there will be probably more checks needed for the formatting of the parser
// TO DO: try to parse one chunk at the time and continue the parsing at the next iter

// POSIBLE LOGIC: add line by line to _bodyTokens and then check the vector, if there is a pair of matching chunkSizeLine and chunkLine
// then parse it, add it to the body and remove them from the vector
void	Parser::parser_chunked(std::string& input) {

	// still have to test this function with inputs that might not end with the delimiter
	addTokens(input, "\r\n");

	// here make check if there is a chunk present, if it is then parse t
	checkForChunks(_request._bodyTokens);

	// std::cout << "here comes the body vector for chunked requests" << std::endl;

	std::cout << _request._bodyTokens.size() << std::endl;
	// printStringVector( _request._bodyTokens);


	// check what is happening with the ending and why is not being added to the body
	if (isChunkedFinished( _request._bodyTokens)) {
		std::cout << "Final chunk detected, chunked parsing finished\n" << std::endl;
		_request._finished = true;
		return;
	}
}

void	Parser::parse_body(std::string& input) {

	// handle also errors when body here is empty
	// this can go wrong if the full body is not already included in the input variable
	std::string body = cleanBody(input);

	if (_request._headers.find(HeaderType::CONTENT_LENGTH) != _request._headers.end())
	{
		std::cout << "parsing unchunked:" << std::endl;
		parser_unchunked(body);
	}
	else if (_request._headers.find(HeaderType::TRANSFER_ENCODING) != _request._headers.end())
	{
		// this will later have to actually check what is the actual value of TRANSFER_ENCODING
		std::cout << "parsing chunked:" << std::endl;
		parser_chunked(body);
	}
	else
	{
		std::cout << "NO BODY\n";
		// note by fabi: updated: there is simply no body I think
		_request._finished = true;
		return ;

		//std::cout << "error parsing body" << std::endl;
		// handle logic for errors
		// if there is no content lenght it can be an error,
	}
}


// call after headers are parsed
bool	Parser::_invalid_headers(void) {
	// don't accept expect header
	if (_request._headers.find(HeaderType::EXPECT) != _request._headers.end()) {
		_request._finished = true;
		_request.set_status_code(417);
		return (true);
	}
	return (false);
}

// to do:
// 1. finish the checking for the end of the body
// 2. do more checking for chunked and unchunked bodies in multiple reads
void Parser::parse(void) {
	// std::cout << "from parser:" <<std::endl << "|" << input << "|" << std::endl;
	//std::cout << "Parsing started\n";
	//std::cout << "parser input: " << _input << "\n";
	RequestArray	array;
	if (!_request._areHeadersParsed)
	{
		try {
			array = RequestArray(_input);
		} catch (const RequestArray::NotTerminated& e) {
			(void)e;
			_request._finished = false;
			return ;
		} catch (const StringArray::NotTerminated& e) {
			(void)e;
			_request._finished = false;
			return ;
		}
		FT_ASSERT(array.size());
		std::cout << array;

		parse_first_line(array);
		if (_request._finished) {
			return ;
		}
		parse_headers(array);
		if (_request._finished) {
			return ;
		}
	}

	if (_request._areHeadersParsed) {
		if (_invalid_headers()) {
			return ;
		}
		if (_config_index == -1) {
			_select_config();
		}
		//todo: if we update to make request_body_size a part of a location config
		//instead of a serve config change this below according
		int allowed_body_size = get_location_config().getRequestBodySize();
		bool body_too_large = false;
		bool invalid_body_size = false;
		try {
			int actual_body_size = std::stoi(_request._headers[HeaderType::CONTENT_LENGTH]);
			body_too_large = allowed_body_size != -1 && actual_body_size > allowed_body_size;
			invalid_body_size = actual_body_size < 0;
		} catch (const std::invalid_argument&) {
			invalid_body_size = true;
		} catch (const std::out_of_range&) {
			invalid_body_size = true;
		}

		if (body_too_large) {
			_request.set_status_code(400);
			_request._finished = true;
			return ;
		} else if (invalid_body_size) {
			_request.set_status_code(500);
			_request._finished = true;
			return ;
		}
		parse_body(_input);
	}

	//_request.displayRequest();
}

// void	Parser::parse_first_line(std::string input) {

// 	std::string first_line = extract_first_line(input);
// 	vector line = split(first_line, ' ');
// 	// this could be later a series of other checks
// 	if (line.size() != 3)
// 		throw std::exception();

// 	setRequestMethod(line[0]);
// 	setUri(line[1]);
// 	setVersion(line[2]);

// 	// std::cout << "size = " << line[0] << std::endl;

// 	print_vector_with_delimiter(line);
// }

/*
std::string Parser::extract_first_line(const std::string& line) {
	size_t pos = line.find("\r\n");
	if (pos != std::string::npos) {
		return line.substr(0, pos);
	}
	return line;
}
*/

void print_vector_with_delimiter(const std::vector<std::string>& vec) {
	for (size_t i = 0; i < vec.size(); ++i) {
		std::cout << "|" << vec[i] << "|";
		if (i != vec.size() - 1) {  // Avoid printing delimiter after the last element
			std::cout << " ";
		}
	}
	std::cout << std::endl;
}

bool Parser::is_header_present(const std::string& str) {
	size_t pos = str.find("\r\n\r\n");

	return pos != std::string::npos;
}

/* to check wether the read request is complete or needs to be read further */
bool	Parser::is_finished(void) {
	if (_request._finished) {
		std::transform(_request._uri.begin(), _request._uri.end(), _request._uri.begin(),
			[](unsigned char c) { return (std::tolower(c));});
	}
	return (_request._finished);
}

Request	Parser::get_request(void) const {
	return (_request);
}


bool Parser::ends_with(const std::string& str, const std::string& suffix) {
	if (str.size() < suffix.size()) {
		return false;
	}
	return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}
