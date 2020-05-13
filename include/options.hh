#ifndef _OPTIONS_HH_
#define _OPTIONS_HH_

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

namespace misc { namespace options {
	
	class option {
	public:
		char short_name;
		std::string long_name;
		std::string description;
		std::string arg_name;
		bool optional;
		size_t required;
		
		option(char short_name, const std::string& long_name, 
			   const std::string& description, const std::string& arg_name = "", 
			   bool optional = false, size_t required = 0)
		: short_name(short_name), long_name(long_name), description(description),
		arg_name(arg_name), optional(optional), required(required) {}
		
		virtual ~option() {}
		
		virtual bool takes_optional_args() const { return optional; }
		virtual size_t num_required_args() const { return required; }
		
		virtual void process(std::vector<std::string>& args) { return; }
		
		std::string get_help(size_t cw, size_t lw) const;
	};
	
	template<typename T>
	class store_option : public option {
	private:
		T& arg;
		
	public:
		store_option(char short_name, const std::string& long_name, 
					 const std::string& description, T& arg, const std::string& arg_name = "", 
					 bool optional = false)
		:	option(short_name, long_name, description, arg_name, optional, optional ? 0 : 1), 
		arg(arg) {
		}
		
		void process(std::vector<std::string>& args) {
			if(!args.empty()) {
				try {
					arg = boost::lexical_cast<T, const std::string>(args[0]);
				} catch (boost::bad_lexical_cast& e) {
					throw std::runtime_error("bad value for option!");
				}
			}
		}
	};
	
	class bool_option : public option {
	private:
		bool& arg;
		bool val;
	public:
		bool_option(char short_name, const std::string& long_name, 
					const std::string& description, bool& arg, const std::string& arg_name, 
					bool optional = true, bool val = true)
		: option(short_name, long_name, description, arg_name, optional, 0), arg(arg), val(val) {
		}
		
		void process(std::vector<std::string>& args) {
			arg = val;
		}
	};
	
	struct help_exception : public std::exception {
	};
	
	class help_option : public option {
	public:
		help_option(char short_name, const std::string& long_name)
		: option(short_name, long_name, "display help and usage") {
		}
		
		void process(std::vector<std::string>& args) {
			throw help_exception();
		}
	};

	struct version_exception : public std::exception {
	};

	class version_option : public option {
	public:
		version_option(char short_name, const std::string& long_name)
		: option(short_name, long_name, "show version information") {
		}

		void process(std::vector<std::string>& args) {
			throw version_exception();
		}
	};
	
	class argument {
	public:
		std::string name;
		std::string description;
		
		argument(const std::string& name, const std::string& description) 
		: name(name), description(description) {}
		
		virtual ~argument() {}
		
		virtual size_t min_num_args() const { return 1; }
		virtual size_t max_num_args() const { return 1; }
		virtual void process(const std::string& val) { return; }
		
		std::string get_usage() const;
		std::string get_help(size_t cw, size_t lw) const;
	};
	
	template<typename T>
	class store_argument : public argument {
	private:
		T& arg;
		
	public:
		store_argument(const std::string& name, const std::string& description, T& arg)
		: argument(name, description), arg(arg) {
		}
		
		void process(const std::string& val) {
			try {
				arg = boost::lexical_cast<T, const std::string>(val);
			} catch (boost::bad_lexical_cast& e) {
				throw std::runtime_error("bad value for argument!");
			}
		}
	};
	
	class parser {
	private:
		std::string name;
		std::string description;
		std::string usage;
		std::string version;

		std::vector<option*> options;
		std::vector<argument*> arguments;
		
		option* find(char short_name) const;
		option* find(const std::string& long_name) const;
		
		bool is_long_option(const std::string& arg) const; 
		bool is_short_option(const std::string& arg) const;
		bool is_end_of_options(const std::string& arg) const;
		
		void parse_long_option(const std::string& arg, const char**& begin, const char**& end) const;
		void parse_short_option(const std::string& arg, const char**& begin, const char**&end) const;
		void parse_option_arguments(option* o, std::vector<std::string>& args, const char**& begin, const char**& end) const;
		
		void collect_option_arguments(option* o, std::vector<std::string>& args, const char**& begin, const char**& end) const;
	public:
		
		parser(const std::string& name, const std::string& description, const std::string& usage = "", const std::string& version = "")
		: name(name), description(description), usage(usage), version(version) {
			add_option(new help_option('?', "help"));
			add_option(new version_option('v', "version"));
		}
		
		~parser();
		
		void print_usage(std::ostream& stream) const;
		void print_help(std::ostream& stream) const;
		void print_version(std::ostream& stream) const;

		void add_option(option* o) { options.push_back(o); }
		void add_argument(argument* a) { arguments.push_back(a); }
		
		template<typename T>
		void add_store_option(char short_name, const std::string& long_name, const std::string& description, 
							  T& arg, const std::string& arg_name = "", bool optional = false);
		
		void add_bool_option(char short_name, const std::string& long_name, const std::string& description,
							 bool& arg, const std::string& arg_name = "", bool optional = true, bool val = true);	
		
		template<typename T>
		void add_store_argument(const std::string& name, const std::string& description, T& arg);
		
		void parse(const char** begin, const char** end, bool handle_errors = true, std::ostream& error_stream = std::cerr);
		
		
	};
	
	template<typename T>
	inline void parser::add_store_option(char short_name, const std::string& long_name, const std::string& description, 
										 T& arg, const std::string& arg_name, bool optional) {
		add_option(new store_option<T>(short_name, long_name, description, arg, arg_name, optional));
	}
	
	template<typename T>
	inline void parser::add_store_argument(const std::string& name, const std::string& description, T& arg) {
		add_argument(new store_argument<T>(name, description, arg));
	}
	
	inline void parser::add_bool_option(char short_name, const std::string& long_name, const std::string& description,
										bool& arg, const std::string& arg_name, bool optional, bool val) {
		add_option(new bool_option(short_name, long_name, description, arg, arg_name, optional, val));
	}
	
} }

#endif
