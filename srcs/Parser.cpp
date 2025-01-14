/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrherna <adrianhdt.2001@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 13:43:05 by adrherna          #+#    #+#             */
/*   Updated: 2025/01/14 14:58:01 by adrherna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/parser/Parser.hpp"
#include <ostream>

void Parser::parse(std::string input) {
	std::cout << "from parser:" <<std::endl << input << std::endl;
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
