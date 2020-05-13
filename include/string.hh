#ifndef _STRING_HH_
#define _STRING_HH_

#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <iterator>

#include <boost/lexical_cast.hpp>

namespace misc { namespace string {

static const std::string whitespace_chars(" \t\n\r\f\v");

struct is_whitespace {
	bool operator()(const char c) const {
		switch(c) {
			case ' ':
			case '\t':
			case '\n':
			case '\r':
			case '\f':
			case '\v':
				return true;
			default:
				return false;
		}	
	}
};

struct uppercaser {
	char operator()(const char c) const {
		return std::toupper(c);
	}
};

struct lowercaser {
	char operator()(const char c) const {
		return std::tolower(c);
	}
};

inline void to_upper(std::string& str) {
	std::transform(str.begin(), str.end(), str.begin(), uppercaser());
}

inline void to_lower(std::string& str) {
	std::transform(str.begin(), str.end(), str.begin(), lowercaser());
}

void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = whitespace_chars);

template<typename output_iterator> 
void split(const std::string& str, output_iterator out, const std::string& sep = whitespace_chars);

template<typename output_iterator>
inline void split(const std::string& str, output_iterator out, const std::string& sep) {
	std::vector<std::string> toks;
	tokenize(str, toks, sep);
	std::transform(toks.begin(), toks.end(), out, boost::lexical_cast<typename output_iterator::container_type::value_type, std::string>); 
} 

template<typename T>
std::string to_string(const T& x) {
	std::ostringstream oss;
	oss << x;
	return oss.str();
}

std::string strip_left(const std::string& str, const std::string& chars = whitespace_chars);
std::string strip_right(const std::string& str, const std::string& chars = whitespace_chars);
std::string strip(const std::string& str, const std::string& chars = whitespace_chars);

std::string ljust(const std::string& s, const std::string::size_type width);
std::string rjust(const std::string& s, const std::string::size_type width);

bool starts_with(const std::string& str, const std::string& prefix, std::string::size_type start = 0, std::string::size_type end = std::string::npos);

std::string fill(const std::string& s, std::string::size_type width = 70, const std::string& initial_indent = "", const std::string& subsequent_indent = "");
void wrap(const std::string& s, std::string& ret, std::string::size_type width = 70, const std::string& initial_indent = "", const std::string& subsequent_indent = "");

int levenshtein_distance(const std::string& a, const std::string& b);

int hamming_distance(const std::string& a, const std::string& b);
	
} }	
#endif
