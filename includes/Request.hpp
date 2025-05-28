/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrherna <adrianhdt.2001@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:03:43 by adrherna          #+#    #+#             */
/*   Updated: 2025/02/04 12:51:15 by adrherna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "enums.hpp"
#include <unordered_map>
#include <vector>

class Request {
	public:
		bool								_finished;
		bool								_areHeadersParsed;
		MethodType							_type;
		std::string							_uri;
		std::string							_version;
		std::unordered_map<HeaderType, std::string>	_headers;
		std::string							_body;
		size_t								_startBodyIdx;
		std::vector<std::string>			_bodyTokens;
		std::string							_response_str;
		std::pair<int, std::string>			_status_code;
		std::vector<std::string>			additional_response_headers;

		Request();
		Request(const Request& old);
		Request(Request&& old);
		Request	operator=(const Request& old);
		Request	operator=(Request&& old);
		~Request() = default;

		MethodType getMethod() const {
			return _type;
		}


		void displayRequest() const {
			std::cout << "Request state:" << std::endl;
			std::cout << "Finished: " << (_finished ? "Yes" : "No") << std::endl;
			std::cout << "Method: " << static_cast<int>(_type) << std::endl;  // Assuming MethodType is an enum, you can print its value
			std::cout << "URI: " << _uri << std::endl;
			std::cout << "Version: " << _version << std::endl;

			std::cout << "Headers:" << std::endl;
			for (const auto& header : _headers) {
				std::cout << "|" << to_string(header.first) << "|" << header.second << "|" << std::endl;
			}
			std::cout << "Body len: " <<  _body.length() << std::endl;
			if (_body.empty()) {
				std::cout << "Body: No body" << std::endl;
			} else {
				std::cout << "Body: " << _body.substr(0, 100);
				if (_body.length() > 100) {
					std::cout << "..";
				}
				std::cout << std::endl;
			}
		}

		void	set_status_code(int code);
};
