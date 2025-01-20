/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrherna <adrianhdt.2001@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 13:43:05 by adrherna          #+#    #+#             */
/*   Updated: 2025/01/20 16:07:20 by adrherna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/parser/Parser.hpp"
#include <cstddef>
#include <vector>

// 1. No Content-Length or Transfer-Encoding in HTTP/1.1
// Connection Closure:
// If neither Content-Length nor Transfer-Encoding is specified, the end of the body is signaled by the server closing the connection.
// This is a fallback mechanism in HTTP/1.1 but is considered bad practice and is rarely used.

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

void printStringArray(const StringArray& arr) {
	for (size_t i = 0; i < arr.size(); ++i) {
		std::cout << "Line " << i + 1 << ": ";
		for (const auto& word : arr[i]) {
			std::cout << word << " ";
		}
		std::cout << std::endl;
	}
}

//splits a string into the individual words and puts it inside a vector. Like in a char **.
std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = str.find(delimiter);

	while (end != std::string::npos) {
		tokens.push_back(str.substr(start, end - start));
		start = end + delimiter.length();
		end = str.find(delimiter, start);
	}

	tokens.push_back(str.substr(start));

	return tokens;
}

// Function to split the input string into arrays (lines and words) and stop at stopDl
// Dl = delimiter
StringArray splitIntoArrays(const String& input, const String& lineDl, const String& stopDl) {
	String str;
	StringArray result;
	size_t start = 0, end = 0;

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

void Parser::setRequestMethod(const std::string& method) {
	if (method == "GET") {
		_request._type = MethodType::GET;
	} else if (method == "POST") {
		_request._type = MethodType::POST;
	} else if (method == "PUT") {
		_request._type = MethodType::PUT;
	} else if (method == "DELETE") {
		_request._type = MethodType::DELETE;
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
	if (str == "Content-Length:")
		return HeaderType::CONTENT_LENGTH;
	else if (str == "Transfer-Encoding:")
		return HeaderType::TRANSFER_ENCODING;
	else if (str == "Connection:")
		return HeaderType::CONNECTION;
	else if (str == "Host:")
		return HeaderType::HOST;
	else
		return HeaderType::INVALID;
}

void Parser::insertHeader(const std::string& key, const std::string& value) {
	HeaderType keyType = setType(key);
	_request._headers.insert({keyType, value});

	std::cout << "Header:" << std::endl;
	std::cout << "key: " << key << " value: " << value << std::endl;
}

void	Parser::parse_first_line(const StringArray& array) {
	if (array[0].size() != 3)
	{
		//handle error logic, first line is incomplete
	}
	setRequestMethod(array[0][0]);
	setUri(array[0][1]);
	setVersion(array[0][2]);
}

void	Parser::parse_headers(const StringArray& array) {
	size_t i = 1;

	while (i < array.size())
	{
		if (array[i].size() != 2)
		{
			// handle logic when headers are not key pair value
		}
		insertHeader(array[i][0], array[i][1]);
		i++;
	}
}

void	Parser::parser_unchunked(std::string& input) {

	unsigned long bytesToRead = 0;
	auto it = _request._headers.find(HeaderType::CONTENT_LENGTH);

	if (it != _request._headers.end()) {
		bytesToRead = std::stoul(it->second);
	} else {
		std::cout << "Content-Length header not found!" << std::endl;
	}

	// what happens if input is smaller than ConLen ? does that mean that we havent finished reading the full body ?
	if (input.size() < bytesToRead) {
		std::cout << "Input is smaller than Content-Length!" << std::endl;
		return;
	}

	for (unsigned int i = 0; i < bytesToRead; ++i) {
		char currentChar = input[i];
		std::cout << currentChar;
		_request._body.push_back(currentChar);
	}

	std::cout << "\nFinished reading unchunked body." << std::endl;
}

void printStringVector(const std::vector<std::string>& vec) {
	for (const auto& str : vec) {
		std::cout << "|" << str << "|" << std::endl;
	}
}

// there will be probably more checks needed for the formatting of the parser
void	Parser::parser_chunked(std::string& input) {

	std::string chunkLine;
	std::string chunkSizeLine;

	std::vector<std::string> bodyVector = split(input, "\r\n");
	std::cout << "here comes the body vector for chunked requests" << std::endl;

	printStringVector(bodyVector);
	std::cout << bodyVector.size() << std::endl;

	if ((bodyVector.size() - 1) % 2 != 0)
	{
		std::cout << "bodyVector size check exited" << std::endl;
		// handle error, this means that there is one line that doesnt also have a line with the size or vice versa
		return;
	}

	for (size_t i = 0; i < bodyVector.size(); i += 2) {
		chunkSizeLine = bodyVector[i];

		std::cout << "chunkSizeLine " << chunkSizeLine << std::endl;

		int chunkSize = sizeLineToInt(chunkSizeLine);

		if (i + 1 < bodyVector.size()) {
			chunkLine = bodyVector [i + 1];
			std::cout << "chunk size = " << chunkSize << " actual chunk size " << chunkLine.size() << std::endl;
			_request._body += chunkLine;
		}
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
		std::cout << "error parsing body" << std::endl;
		// handle logic for errors
		// if there is no content lenght it can be an error,
	}
}

void Parser::parse(std::string input) {
	std::cout << "from parser:" <<std::endl << input << std::endl;

	StringArray array = splitIntoArrays(input, "\r\n", "\r\n\r\n");
	printStringArray(array);
	std::cout << std::endl;

	parse_first_line(array);
	parse_headers(array);
	parse_body(input);

	_request.displayRequest();
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


std::string Parser::extract_first_line(const std::string& line) {
	size_t pos = line.find("\r\n");
	if (pos != std::string::npos) {
		return line.substr(0, pos);
	}
	return line;
}

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
bool	Parser::is_finished(void) const {
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


/* once is_finished is true this can be called.
 * Resets the parser/lexer for the next client request with the same
 	connection.
 * todo: think of way to handle persistant connection (andvance input and reuse
	it
*/
// void	Parser::_reset(void) {

// /* once is_finished is true this can be called.
//  * Resets the parser/lexer for the next client request with the same
//  	connection.
// */_request.finished = false;
// 	/* todo: reset every field in _request */
// }
