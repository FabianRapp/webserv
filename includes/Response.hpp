/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrherna <adrianhdt.2001@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 13:00:16 by adrherna          #+#    #+#             */
/*   Updated: 2025/01/29 15:38:08 by adrherna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include "FdClasses/Server.hpp"
#include "enums.hpp"
#include <unordered_map>
#include <vector>
#include "ConfigParser/ConfigParser.hpp"
#include "ConfigParser/LocationConfigFile.hpp"
#include "Request.hpp"

class Client;

class Response {
	private:
		std::string			_readContent;
		std::string			_body;
		ServerConfigFile	_config;
		std::string			_target;
		Server*				_server;

	public:
		Response ();
		Response (const ServerConfigFile& configFile, const Request& request, Client& client);
		~Response ();
		Response operator=(const Response& old);

		std::string& getBody();
		std::string& getReadContent();
		std::string& getTarget();
		ServerConfigFile& getConfig();

		void	appendToStatusLine(std::string content);
		void	appendToBody(std::string content);
		void	appendToRead(std::string content);
		std::string	getExpandedTarget(Request& request);
};

#endif