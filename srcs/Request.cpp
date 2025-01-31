/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrherna <adrianhdt.2001@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 20:28:01 by adrherna          #+#    #+#             */
/*   Updated: 2025/01/22 14:02:45 by adrherna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"

Request::Request() {
	_finished = false;
	_areHeadersParsed = false;
	_type = MethodType::INVALID;
	_uri = "";
	_version = "";
	_body = "";
	_startBodyIdx = 0;
}
Request::Request(const Request& old)
:	_finished(old._finished),
	_areHeadersParsed(old._areHeadersParsed),
	_type(old._type),
	_uri(old._uri),
	_version(old._version),
	_body(old._body),
	_startBodyIdx(old._startBodyIdx)
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
	_body = old._body;
	_startBodyIdx = old._startBodyIdx;

	return (*this);
}
