/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrherna <adrianhdt.2001@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 13:00:16 by adrherna          #+#    #+#             */
/*   Updated: 2025/02/04 13:47:25 by adrherna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <unistd.h>
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
#include "CGIManager.hpp"
#include <iostream>
#include <filesystem>

class Client;
enum class ClientMode;


class Response {
	public:
		enum class ResponseMode {
			NORMAL,
			FINISH_UP
		};
	private:

		std::string				_response_str;
		const Request&			_request;
		ClientMode&				_client_mode;
		std::string				_body;
		const ServerConfigFile&	_config;
		const LocationConfigFile&	_location_config;
		std::string				_target;
		Server*					_server;
		Client*					_client;
		std::string				_path;
		std::vector<MethodType> _allowedMethods;

		WriteFd*				_writer;
		ReadFd*					_reader;
		ResponseMode			_mode;
		CGIManager*				_cgi_manager;

		std::string_view		_fd_write_data;
		bool					_first_iter;
		DIR*					_dir;


	void						_handle_get_moved(const std::string& new_loc);
	std::vector<std::string>	_get_dir(void);
	std::string					_auto_index_body(std::vector<std::string>& files);
	void						_handle_auto_index(std::vector<std::string>& files);
	void	_handle_post(void);
	void	_handle_put(void);
	void	_handle_delete(void);
	void	_handle_get(void);
	void	_handle_get_file(void);
	bool	_has_index(std::vector<std::string>& files, std::string& index_file);
	void	_handle_put_file(bool post);
	void	_append_content_type(const std::string& path);
	void	_handle405(void);

	void	_finish_up(void);
public:
	bool	in_error_handling;
		Response () = delete;
		Response(const ServerConfigFile& configFile, const LocationConfigFile& locationConfig,
			const Request& request,Client& client, ClientMode& client_mode);
		~Response();
		Response operator=(const Response& old);

		std::string&&	get_str_response(void);
		WriteFd*&		get_writer(void);
		CGIManager*&	get_cgi_manger(void);

		void			load_status_code_response(int code, const std::string& status);
		void			load_status_code_body(int code);

		void			set_mode(ResponseMode mode);
		void			read_fd(int read_fd, ssize_t byte_count);
		void			write_fd(int write_fd);
		void			reset_body(void);
		std::string&&	get_read_fd_data(void);
		void			set_fd_write_data(const std::string_view data);
		std::string& getBody();
		std::string& getTarget();
		const ServerConfigFile& getConfig() const;
		void	execute(void);
	


		void	appendToStatusLine(std::string content);
		void	appendToBody(std::string content);
		std::string	getExpandedTarget(void);
		void setAllowedMethods();

		bool isMethodAllowed(MethodType method);
		// std::string expandPath();

	// call this from client in case of early destruction
	void	close_io_fds(void);
};

#endif
