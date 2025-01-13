/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrherna <adrherna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 20:28:01 by adrherna          #+#    #+#             */
/*   Updated: 2025/01/13 20:38:46 by adrherna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Request.hpp>

Request::Request() {
    _finished = false;
    _type = MethodType::INVALID;
    _uri = "";
    _version = "";
    _body = nullptr;
}