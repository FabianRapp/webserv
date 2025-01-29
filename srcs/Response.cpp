/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrherna <adrianhdt.2001@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 13:09:21 by adrherna          #+#    #+#             */
/*   Updated: 2025/01/29 13:15:43 by adrherna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"
#include "../includes/FdClasses/Client.hpp"


Response::Response()
{

}

Response::Response(const ServerConfigFile& configFile, const Request& request,Client& client) : _config(configFile) {
	//later expansions have to be applied if upload_dir is present
	_target = request._uri;
	_readContent = "";
	_body = "";
	_server = client.server;
}

std::string& Response::getBody() {
	return(_body);
}

std::string& Response::getReadContent() {
	return (_readContent);
}

std::string& Response::getTarget() {
	return (_target);
}

ServerConfigFile& Response::getConfig() {
	return (_config);
}

void	Response::appendToBody(std::string content) {
	_body += content;
}

void	Response::appendToRead(std::string content) {
	_readContent += content;
};

std::string&	Response::expandTarget(Request& request) {
	// has yet to be implemented
	return _target;
}