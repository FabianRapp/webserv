#include <StringArray.hpp>

Line::Line(const String& full_line, const String& delimiter, int word_count):
	std::vector<String>(),
	_full_line(full_line)
{
	const bool debug = false;

	if (debug) {std::cout << "line in: " << full_line << "\n";}
	size_t start = 0;
	size_t end = _full_line.find(delimiter);

	while (end != String::npos && word_count--) {
		String token;
		if (end != start) {
			token = _full_line.substr(start, end - start);
			if (debug) {std::cout << "token: |" << token << "|\n";}
			push_back(token);
		} else {
			//// remove this to not have delimiters in array
			//token = delimiter;
			//if (debug) {std::cout << "token: |" << token << "|\n";}
			//push_back(token);
			end++;
		}
		start = end;
		end = _full_line.find(delimiter, start);
	}
	// push last word
	if (start != _full_line.size()) {
		String token = _full_line.substr(start, _full_line.size() - start);
		if (debug) {std::cout << "token: |" << token << "|\n";}
		push_back(token);
	}
}

Line::Line(const String& full_line, const String& delimiter):
	Line(full_line, delimiter, -1)
{}

Line::Line(const Line& old):
	std::vector<String>(old),
	_full_line(old._full_line)
{
}

Line	Line::operator=(const Line& old) {
	if (this == &old) {
		return (*this);
	}
	std::vector<String>::operator=(old);
	_full_line = old._full_line;
	return (*this);
}

const String&	Line::get_full_line(void) const {
	return (_full_line);
}

std::ostream	&operator<<(std::ostream &output, const Line &line) {
	output << "size: " << line.size() << "\t\t|";
	for (const auto& word : line) {
		output << word << "\t|";
	}
	return (output);
}

StringArray::StringArray(const String& input, String lineDl,
		String stopDl):
	std::vector<Line>(),
	_full_input(input)
{
	String str;
	size_t start = 0, end = 0;
	const bool	debug = false;
	
	if (debug) {std::cout << "input: " << input << "\n";}
	end = input.find(stopDl);
	if (end == String::npos) {
		std::cout << "could not find stopDl" << std::endl;
		throw (StringArray::NotTerminated());
	}

	str = input.substr(start, end);

	while ((end = str.find(lineDl, start)) != String::npos) {
		Line line(str.substr(start, end - start), " ");
		if (debug) { std::cout << "line: " << line << "\n";}
		push_back(line);
		start = end + lineDl.length();
	}

	if (start < str.length()) {
		Line lastLine(str.substr(start), " ");
		if (debug) { std::cout << "line: " << lastLine << "\n";}
		push_back(lastLine);
	}

	// remove/add this if the final /r/n/r/n shouldn't be in the arr
	// _lines.push_back(stopDl);
}

StringArray::StringArray(const StringArray& old):
	std::vector<Line>(old),
	_full_input(old._full_input)
{}

const String&	StringArray::get_input(void) const {
	return (_full_input);
}

StringArray	StringArray::operator=(const StringArray& old) {
	std::vector<Line>::operator=(old);
	if (this == &old) {
		return (*this);
	}
	_full_input = old._full_input;
	return (*this);
}

std::ostream	&operator<<(std::ostream &output, const StringArray &arr) {
	output << "String array:\n";
	for (size_t idx = 0; idx < arr.size(); idx++) {
		output << idx << ": " << arr[idx] << '\n';
	}
	return (output);
}

//too: catch line exceptions
RequestArray::RequestArray(String& input):
	std::vector<Line>(),
	_full_input(input)
{
	String lineDl = "\r\n";
	String stopDl = "\r\n\r\n";
	String str;
	size_t start = 0, end = 0;
	const bool	debug = false;
	
	//if (debug) {std::cout << "input: " << input << "\n";}
	end = input.find(stopDl);
	if (end == String::npos) {
		std::cout << "could not find stopDl" << std::endl;
		throw (StringArray::NotTerminated());
	}
	//if (input.find("\r\n") != std::string::npos) {
	//	ssize_t	end_first_line = static_cast<ssize_t>(input.find("\r\n"));
	//	std::transform(input.begin() + end_first_line, input.begin() + static_cast<ssize_t>(end), input.begin(),
	//		[](unsigned char c) { return (std::tolower(c));});

	//}

	str = input.substr(start, end);

	if ((end = str.find(lineDl, start)) != String::npos) {
		Line line(str.substr(start, end - start), " ", 3);
		if (debug) { std::cout << "line: " << line << "\n";}
		push_back(line);
		start = end + lineDl.length();
	}

	while ((end = str.find(lineDl, start)) != String::npos) {
		Line line(str.substr(start, end - start), " ", 2);
		if (debug) { std::cout << "line: " << line << "\n";}
		push_back(line);
		start = end + lineDl.length();
	}

	if (start < str.length()) {
		Line lastLine(str.substr(start), " ", 2);
		if (debug) { std::cout << "line: " << lastLine << "\n";}
		push_back(lastLine);
	}
	// remove/add this if the final /r/n/r/n shouldn't be in the arr
	// _lines.push_back(stopDl);
}

RequestArray::RequestArray(const RequestArray& old):
	std::vector<Line>(old),
	_full_input(old._full_input)
{}

const String& RequestArray::get_input(void) const {
	return (_full_input);
}

RequestArray	RequestArray::operator=(const RequestArray& old) {
	std::vector<Line>::operator=(old);
	if (this == &old) {
		return (*this);
	}
	_full_input = old._full_input;
	return (*this);
}

std::ostream	&operator<<(std::ostream &output, const RequestArray &arr) {
	output << "Request array:\n";
	for (size_t idx = 0; idx < arr.size(); idx++) {
		output << idx << ": " << arr[idx] << '\n';
	}
	return (output);
}

