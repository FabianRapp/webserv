/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrherna <adrianhdt.2001@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:03:43 by adrherna          #+#    #+#             */
/*   Updated: 2025/01/20 18:20:27 by adrherna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "enums.hpp"
#include <unordered_map>

using HeadersMap = std::unordered_map<HeaderType, std::string>;

class Request {
	public:
		bool				_finished;
		MethodType			_type;
		std::string			_uri;
		std::string			_version;
		HeadersMap			_headers;
		std::string			_body;

		Request();
		~Request() = default;

		void displayRequest() const {
			std::cout << "Request state:" << std::endl;
			std::cout << "Finished: " << (_finished ? "Yes" : "No") << std::endl;
			std::cout << "Method: " << static_cast<int>(_type) << std::endl;  // Assuming MethodType is an enum, you can print its value
			std::cout << "URI: " << _uri << std::endl;
			std::cout << "Version: " << _version << std::endl;

			// std::cout << "Headers:" << std::endl;
			// for (const auto& header : _headers) {
			// 	std::cout << "|" << header.first << "|" << header.second << "|" << std::endl;
			// }
			std::cout << "Body len: " <<  _body.length() << std::endl;
			std::cout << "Body: " << (_body.empty() ? "No body" : _body) << std::endl;
		}

};