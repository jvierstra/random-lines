#include <stdexcept>
#include <cassert>
#include <climits>
#include <algorithm>

#include "options.hh"
#include "string.hh"

using namespace misc::string;


namespace misc { namespace options {
	
	
	std::string option::get_help(size_t cw, size_t lw) const {
		std::string ret(2, ' ');
		if (short_name) {
			ret.append("-");
			ret.append(1, short_name);
		} else {
			ret.append(4, ' ');
		}
		
		if (not long_name.empty()) {
			if (short_name) {
				ret.append(", ");
			}
			ret.append("--");
			ret.append(long_name);
		}
		
		if (takes_optional_args() || num_required_args() > 0) {
			if (long_name.empty()) {
				ret.append(arg_name);
			} else {
				ret.append("=");
				ret.append(arg_name);
			}
		}
		
		if (ret.length() < cw) {
			ret = ljust(ret, cw);
		} else {
			ret.append(4, ' ');
		}
		
		return fill(description, lw, ret, std::string(cw, ' '));
	}
	
	
	std::string argument::get_usage() const {
		std::string usage;
		if (min_num_args() == max_num_args()) {
			for (size_t i = min_num_args(); i <= max_num_args(); ++i) {
				if (i != min_num_args()) {
					usage += " ";
				}
				usage += name;
			}
		} else if (min_num_args() == 0 && max_num_args() == std::numeric_limits<size_t>::max()) {
			usage = "[" + name + "]*";
		} else {
			if (min_num_args() == 0) {
				usage = "[" + name + "]";
			} else {
				usage = name;
			}
			usage += "{";
			if (min_num_args() != 0) {
				usage += min_num_args();
			}
			usage += ",";
			if (max_num_args() != std::numeric_limits<size_t>::max()) {
				usage += max_num_args();
			}
			usage += "}";
		}
		return usage;
	}
	
	std::string argument::get_help(size_t cw, size_t lw) const {
		std::string ret(2, ' ');
		ret.append(name);
		
		if (ret.length() < cw) ret = ljust(ret, cw);
		else ret.append(4, ' ');
		
		if (description.empty()) {
			return fill(ret, cw, std::string(2, ' '), std::string(2, ' '));
		} else {
			return fill(description, lw, ret, std::string(cw, ' '));
		}
	}
	
	parser::~parser() {
		//clean up options
		for(std::vector<option*>::iterator it = options.begin(); it != options.end(); ++it) {
			delete *it;
		}
		//clean up arguments
		for(std::vector<argument*>::iterator it = arguments.begin(); it != arguments.end(); ++it) {
			delete *it;
		}
	}
	
	void parser::print_usage(std::ostream& stream) const {
		stream << name << " [options]";
		for (std::vector<argument*>::const_iterator it = arguments.begin(); it != arguments.end(); ++it) {
			if (not (*it)->name.empty()) {
				stream << ' ' << (*it)->get_usage();
			}
		}
		stream << ' ' << usage << std::endl;
	}
	

	void parser::print_help(std::ostream& stream) const {
		const size_t lw = (getenv("COLUMNS") == NULL ?	80 : atoi(getenv("COLUMNS")));
		const size_t cw = 30;
		
		print_usage(stream);

		if(not description.empty()) {
			stream << fill(description, lw);
		}

		if(not version.empty()) {
			stream << "version " << version << '\n';
		}

		if (!arguments.empty()) {
			stream << '\n' << "Arguments: " << '\n';
			for (std::vector<argument*>::const_iterator it = arguments.begin();
				 it != arguments.end(); ++it) {
				if (!(*it)->description.empty()) {
					stream << (*it)->get_help(cw, lw);
				}
			}
		}
		
		//print options
		if (!options.empty()) {
			stream << '\n' << "Options: " << '\n';
			for (std::vector<option*>::const_iterator it = options.begin();
				 it != options.end(); ++it) {
				stream << (*it)->get_help(cw, lw);
			}
		}
	};
	
	void parser::print_version(std::ostream& stream) const {

		stream << name << " version " << version << std::endl;

	}

	bool parser::is_long_option(const std::string& arg) const {
		return arg.length() > 2 && arg[0] == '-' && arg[1] == '-';
	}
	
	bool parser::is_short_option(const std::string& arg) const {
		return arg.length() > 1 && arg[0] == '-' && arg[1] != '-';
	}
	
	bool parser::is_end_of_options(const std::string& arg) const {
		return arg == "--";
	}
	
	option* parser::find(char short_name) const {
		for (std::vector<option*>::const_iterator it = options.begin();
			 it != options.end(); ++it) {
			if ((*it)->short_name == short_name) {
				return *it;
			}
		}
		throw std::runtime_error("Invalid option!");
	}
	
	option* parser::find(const std::string& long_name) const {
		option* o = NULL;
		for (std::vector<option*>::const_iterator it = options.begin();
			 it != options.end(); ++it) {
			if ((*it)->long_name == long_name) {
				return *it;
			} else if (starts_with((*it)->long_name, long_name)) {
				if (not o) {
					o = *it;
				} else {
					throw std::runtime_error("Ambiguous option!");
				}
			}
		}
		
		if (not o) {
			throw std::runtime_error("Invalid option!");	
		}
		
		return o;
	}
	
	void parser::collect_option_arguments(option* o, std::vector<std::string>& args, const char**& begin, const char**& end) const {
		size_t args_added = 0;
		if (!o->takes_optional_args() && args.size() == 1) {
			args_added = 1;
		}
		while (args_added < o->num_required_args() && begin != end) {
			args.push_back(*begin);
			++begin;
			++args_added;
		}
		if (args_added != o->num_required_args()) {
			throw std::runtime_error("Wrong number arguments for given option: " + o->long_name);
		}
	}
	
	void parser::parse_long_option(const std::string& arg, const char**& begin, const char**& end) const {
		std::vector<std::string> args;
		std::string::const_iterator equal_sign_pos = std::find(arg.begin() + 1, arg.end(), '=');
		std::string name(arg.begin(), equal_sign_pos);
		option* o = find(name);
		if (equal_sign_pos != arg.end()) {
			args.push_back(std::string(equal_sign_pos + 1, arg.end()));
		}
		collect_option_arguments(o, args, begin, end);
		o->process(args);
	}
	
	void parser::parse_short_option(const std::string& arg, const char**& begin, const char**& end) const {
		std::vector<std::string> args;
		for (std::string::const_iterator it = arg.begin(); it != arg.end(); ++it) {
			option* o = find(*it);
			if (!o->takes_optional_args() && o->num_required_args() == 0) {
				o->process(args);
			} else {
				++it;
				if (it != arg.end()) {
					args.push_back(std::string(it, arg.end()));
				}
				collect_option_arguments(o, args, begin, end);
				o->process(args);
				break;
			}
		}
	}
	
	void parser::parse(const char** begin, const char** end, bool handle_errors, std::ostream& error_stream) {
		assert(begin != end);
		++begin;
		
		size_t arg_count = 0;
		std::vector<argument*>::iterator it = arguments.begin();
		
		bool process = true;
		
		try {
			while (begin != end) {
				std::string arg = *begin;	
				++begin;
				
				if (process && is_end_of_options(arg)) {
					process = false;
				} else if (process && is_short_option(arg)) {
					parse_short_option(arg.substr(1), begin, end);
				} else if (process && is_long_option(arg)) {
					parse_long_option(arg.substr(2), begin, end);
				} else if (it != arguments.end()) {
					(*it)->process(arg);
					++arg_count;
					if ((*it)->max_num_args() == arg_count) {
						arg_count = 0;
						++it;
					}
				} else {
					throw std::runtime_error("Too many or too few arguments!");
				}
			
			}

			if (it != arguments.end() && arg_count < (*it)->min_num_args()) {
				throw std::runtime_error("Too many or too few arguments!");
			}

		} catch (help_exception& e) {
			print_help(error_stream);
			exit(0);
		} catch (version_exception& e) {
			print_version(error_stream);
			exit(0);
		}	catch (std::exception& e) {
			error_stream << "Error: " << e.what() << std::endl;
			print_help(error_stream);
			exit(-1);
		}
	}
	
} }
