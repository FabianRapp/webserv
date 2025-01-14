/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrherna <adrianhdt.2001@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:03:43 by adrherna          #+#    #+#             */
/*   Updated: 2025/01/14 14:57:35 by adrherna         ###   ########.fr       */
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
};