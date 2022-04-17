#pragma once

#include <utils/list.h>

class argparser {
	public:
		argparser(char* args);
		bool is_arg(const char* arg);
		char* get_arg(const char* arg);
		
	private:
		struct arg_node {
			char _name[64];
			char _value[64];
			bool used;
		};

		list<arg_node> args_list;
};

extern argparser* global_argparser;
void setup_global_argparser();