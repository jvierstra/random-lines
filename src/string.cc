#include "string.hh"

namespace misc { namespace string {

void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters) {
  std::string::size_type lastpos = str.find_first_not_of(delimiters, 0);
  std::string::size_type pos = str.find_first_of(delimiters, lastpos);
  
  while (std::string::npos != pos || std::string::npos != lastpos) {
      tokens.push_back(str.substr(lastpos, pos - lastpos));
      lastpos = str.find_first_not_of(delimiters, pos);
      pos = str.find_first_of(delimiters, lastpos);
	}
}

std::string ljust(const std::string& s, const std::string::size_type width) {
	if (s.length() >= width) {
		return s;
	}
	std::string ljs(width, ' ');
	ljs.replace(0, s.length(), s);
	return ljs;
}

std::string rjust(const std::string& s, const std::string::size_type width) {
	if (s.length() >= width) {
		return s;
	}
	std::string rjs(width, ' ');
	rjs.replace(width - s.length(), s.length(), s);
	return rjs;
}		

std::string strip_left(const std::string& str, const std::string& chars) {
	std::string::size_type pos = str.find_first_not_of(chars);
	return (pos == std::string::npos ? "" : str.substr(pos));
}

std::string strip_right(const std::string& str, const std::string& chars) {
	std::string::size_type pos = str.find_last_not_of(chars);
	return (pos == std::string::npos ? "" : str.substr(0, pos + 1));	
}

std::string strip(const std::string& str, const std::string& chars) {
	return strip_right(strip_left(str, chars), chars);
}

bool starts_with(const std::string& str, const std::string& prefix, std::string::size_type start, std::string::size_type end) {
	std::string::const_iterator	str_begin = str.begin() + start, str_end = (end == std::string::npos ? str.end() : str.begin() + end), pre_begin = prefix.begin(), pre_end = prefix.end();
	while (str_begin != str_end && pre_begin != pre_end && *str_begin == *pre_begin) {
		++str_begin;
		++pre_begin;
	}
	return (pre_begin == pre_end);
}

void wrap(const std::string& s, std::string& ret, std::string::size_type width, const std::string& initial_indent, const std::string& subsequent_indent) {
	//tokenize
	std::vector<std::string> tokens;
	std::vector<std::string>::const_iterator word;
	tokenize(s, tokens);
	
	std::string line;
	line.reserve(width);
	line.append(initial_indent);
	
	bool firstWord = true;

	for (word = tokens.begin(); word != tokens.end(); ++word) {
		if (line.length() + 1 + word->length() > width) {
			if (firstWord) {
				ret += *word + "\n";
				line.clear();
				line.append(subsequent_indent);
			} else {
				ret += line + "\n";
				line.clear();
				line.append(subsequent_indent);
				line.append(*word);
			}
		} else {
			if (firstWord) {
				line.append(*word);
				firstWord = false;
			} else {
				line.append(" ");
				line.append(*word);
			}
		}
	}
	
	if (!firstWord) {
		ret += line + "\n";
	}
}

std::string fill(const std::string& s, std::string::size_type width, const std::string& initial_indent, const std::string& subsequent_indent) {
	std::string joined;
	wrap(s, joined, width, initial_indent, subsequent_indent);
	return joined;
}

int levenshtein_distance(const std::string& a, const std::string& b) {	
	int alen = a.length();
	int blen = b.length();
	
	std::vector<std::vector<int> > dpmat(alen+1);
	
	for(int i = 0; i <= alen; ++i) {
		dpmat[i].resize(blen+1);
	}
		
	for(int i = 0; i <= alen; ++i) {
		dpmat[i][0] = i;
	}
	
	for(int j = 0; j <= blen; ++j) {
		dpmat[0][j] = j;
	}
	
	for(int i = 1; i <= alen; ++i) {
		const char ai = a[i-1];
		for(int j = 1; j <= blen; j++) {
			const char bj = b[j-1];
			
			int c;
			if(ai == bj) c = 0;
			else c = 1;
			
			const int above = dpmat[i-1][j];
			const int left = dpmat[i][j-1];
			const int diag = dpmat[i-1][j-1];
			const int cell = std::min(above+1, std::min(left+1, diag+c));
			
			dpmat[i][j] = cell;
		}
	}
	return dpmat[alen][blen];
}

int hamming_distance(const std::string& a, const std::string& b) {
	
	int res = 0;

	for(int i = 0; i < a.size(); ++i) { 
		res += (a[i] == b[i]) ? 0 : 1;
	}
	
	return res;
}

} }
