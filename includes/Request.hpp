/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrherna <adrherna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:03:43 by adrherna          #+#    #+#             */
/*   Updated: 2025/01/13 20:39:01 by adrherna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <enums.hpp>
#include <unordered_map>

using HeadersMap = std::unordered_map<HeaderType, std::string>;

class Request {
	private:
		bool				_finished;
		MethodType			_type;
		std::string 		_uri;
		std::string     	_version;
        HeadersMap	        _headers;
		std::string			_body;

	public:
		Request();
		~Request() = default;
};