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
#include "FdClasses/ReadFd.hpp"
#include "FdClasses/WriteFd.hpp"
#include "Manager.hpp"
#include "enums.hpp"
#include <unordered_map>
#include <vector>
#include "ConfigParser/ConfigParser.hpp"
#include "ConfigParser/LocationConfigFile.hpp"
#include "Request.hpp"

class Client;
enum class ClientMode;

class Response {
	private:
		std::string				_response_str;
		const Request&			_request;
		ClientMode&				_client_mode;
		std::string				_body;
		const ServerConfigFile&	_config;
		std::string				_target;
		Server*					_server;
		std::string				_path;
		bool					_is_cgi;

		size_t					_write_pos;
		WriteFd*				_writer;
		ReadFd*					_reader;
		enum class ResponseMode {
			NORMAL,
			FINISH_UP
		}	_mode;

	std::string_view		_fd_write_data;

	void	_read_fd(int read_fd, ssize_t byte_count, bool close_fd);
	void	_write_fd(int write_fd, bool close_fd);

	void						_load_status_code(int code);
	void						_handle_get_moved(void);
	std::vector<std::string>	_get_dir(void);
	std::string					_auto_index_body(std::vector<std::string>& files);
	void						_handle_auto_index(std::vector<std::string>& files);
	void	_handle_post(void);
	void	_handle_delete(void);
	void	_handle_get(void);
	void	_handle_get_file(void);

	public:
		Response () = delete;
		Response(const ServerConfigFile& configFile, const Request& request,Client& client,
			ClientMode& client_mode);
		~Response();
		Response operator=(const Response& old);

		std::string&&	get_str_response(void);

		std::string& getBody();
		std::string& getTarget();
		const ServerConfigFile& getConfig() const;
		void	execute(void);

		void	appendToStatusLine(std::string content);
		void	appendToBody(std::string content);
		std::string	getExpandedTarget(void);
};

#endif
