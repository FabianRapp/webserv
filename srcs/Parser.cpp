/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrherna <adrianhdt.2001@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 13:43:05 by adrherna          #+#    #+#             */
/*   Updated: 2025/01/17 15:23:20 by adrherna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/parser/Parser.hpp"
#include <cstddef>

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
std::vector<std::string> split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = str.find(delimiter);

	while (end != std::string::npos) {
		tokens.push_back(str.substr(start, end - start));
		start = end + 1;
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
		Line lineV = split(line, ' ');
		result.push_back(lineV);

		start = end + lineDl.length();
	}

	if (start < str.length()) {
		String lastLine = str.substr(start);
		Line lastLineV = split(lastLine, ' ');
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

void Parser::insertHeader(const std::string& key, const std::string& value) {
	_request._headers.insert({key, value});
}

void Parser::parse(std::string input) {
	std::cout << "from parser:" <<std::endl << input << std::endl;

	StringArray array = splitIntoArrays(input, "\r\n", "\r\n\r\n");

	parse_first_line(array);
	parse_headers(array);
	printStringArray(array);
	_request.displayRequest();
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
