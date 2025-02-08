#pragma once

#include "../Request.hpp"
#include "../enums.hpp"
#include "../macros.h"
#include <cstddef>
#include <string>
#include <iostream>
#include <exception>
#include <ostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <ServerConfigFile.hpp>
#include <LocationConfigFile.hpp>

#include "StringArray.hpp"

using String = std::string;

class Parser {
	private:
		Request									_request;
		std::string&							_input;
		int										_max_request_body_size;
		const std::vector<ServerConfigFile>&	_server_configs;
		int										_config_index;//-1 indicates condigs were not set
		int										_location_index;// -1 indicates default location or not set


		void	_select_server_config(void);
		void	_select_location_config(void);
		void	_select_config(void);
	public:

		Parser(std::string& input, const std::vector<ServerConfigFile>& configs);
		~Parser() = default;

		void	parse(void);
		void	parse_first_line(const RequestArray& array);
		void	parse_headers(const RequestArray& array);
		void	parse_body(std::string& input);
		void	parser_unchunked(std::string& input);
		void	parser_chunked(std::string& input);
		void	parse_chunk(std::string& input);

		const ServerConfigFile&	get_config(void) const;
		const LocationConfigFile&	get_location_config(void) const;

		// Setters
		void set_max_request_body_size(int max_request_body_size);
		void setRequestMethod(const std::string& method);
		void setUri(const std::string& uri);
		void setVersion(const std::string& version);

		// Utils
		// std::string	cleanBody(const std::string& input);
		int			getErrorCode();
		void		checkForChunks(std::vector<std::string>& bodyVector);
		void		addTokens(const std::string& str, const std::string& delimiter);
		void		insertHeader(const std::string& key, const std::string& value);
		static bool	is_header_present(const std::string& str);
		std::string extract_first_line(const std::string& request);
		// std::vector<std::string> split(const std::string& str, char delimiter);
		bool		ends_with(const std::string& str, const std::string& suffix);
		bool		is_finished(void);
		Request		get_request(void) const;
};
