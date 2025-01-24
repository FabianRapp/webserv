#pragma once
#include <string>
#include <vector>
#include "../general_includes.hpp"

using String = std::string;

class Line: public std::vector<String> {
	String				_full_line;
public:
	Line(const String& full_line);
	Line(const Line& old);
	~Line(void) = default;
	Line			operator=(const Line& old);
	const String&	get_full_line(void) const;
};

std::ostream	&operator<<(std::ostream &output, const Line &line);


class StringArray: public std::vector<Line> {
	String				_full_input;
public:
	StringArray(void) = default;
	StringArray(const String& input, std::string lineDl, std::string stopDl);
	StringArray(const StringArray& old);
	~StringArray(void) = default;
	StringArray		operator=(const StringArray& old);
	const String&	get_input(void) const;
	class NotTerminated: public std::exception {
	};
};

std::ostream	&operator<<(std::ostream &output, const StringArray &arr);

