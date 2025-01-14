/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrherna <adrianhdt.2001@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 13:43:05 by adrherna          #+#    #+#             */
/*   Updated: 2025/01/14 16:53:24 by adrherna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/parser/Parser.hpp"
#include <ostream>
#include <vector>

bool Parser::ends_with(const std::string& str, const std::string& suffix) {
	if (str.size() < suffix.size()) {
		return false;
	}
	return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

bool Parser::is_header_present(const std::string& str) {
	size_t pos = str.find("\r\n\r\n");

	return pos != std::string::npos;
}

std::vector<std::string> Parser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);

    // Iterate over the string, extracting tokens between the delimiter
    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }

    // Add the last token (or the whole string if no delimiter was found)
    tokens.push_back(str.substr(start));

    return tokens;
}

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

void	Parser::parse_first_line(std::string input) {

	std::string first_line = extract_first_line(input);
	vector line = split(first_line, ' ');

	print_vector_with_delimiter(line);
}

void Parser::parse(std::string input) {
	std::cout << "from parser:" <<std::endl << input << std::endl;
	parse_first_line(input);

}

/* to check wether the read request is complete or needs to be read further */
bool	Parser::is_finished(void) const {
	return (_request._finished);
}

Request	Parser::get_request(void) const {
	return (_request);
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
