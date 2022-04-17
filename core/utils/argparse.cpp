#include <utils/argparse.h>

#include <utils/log.h>

#include <boot/boot.h>

argparser::argparser(char* args) : args_list(10) {
	char* last_token = args;

	int len = strlen(args);

	if (len == 0) {
		return;
	}

	for (int i = 0; i < len; i++) {
		if (args[i] == ' ') {
			args[i] = 0;

			char* starting_assignment = nullptr;

			for (int k = 0; k < strlen(last_token); k++) {
				if (last_token[k] == '=') {
					last_token[k] = 0;
					starting_assignment = &last_token[k + 1];
					break;
				}
			}

			debugf("Found argument: %s (value: '%s')\n", last_token, starting_assignment ? starting_assignment : (char*) "\0");

			arg_node new_node = {
				//._name = *last_token,
				//._value = starting_assignment ? *starting_assignment : *(char*) "\0",
				.used = false
			};

			memcpy(new_node._name, last_token, strlen(last_token));
			if (starting_assignment) {
				memcpy(new_node._value, starting_assignment, strlen(starting_assignment));
			} else {
				memcpy(new_node._value, "\0", 1);
			}

			args_list.add(new_node);

			last_token = &args[i + 1];
		}
	}

	char* starting_assignment = nullptr;

	for (int k = 0; k < strlen(last_token); k++) {
		if (last_token[k] == '=') {
			last_token[k] = 0;
			starting_assignment = &last_token[k + 1];
			break;
		}
	}

	debugf("Found argument: %s (value: '%s')\n", last_token, starting_assignment ? starting_assignment : (char*) "\0");

	arg_node new_node = {
		//._name = *last_token,
		//._value = starting_assignment ? *starting_assignment : *(char*) "\0",
		.used = false
	};

	memcpy(new_node._name, last_token, strlen(last_token));
	if (starting_assignment) {
		memcpy(new_node._value, starting_assignment, strlen(starting_assignment));
	} else {
		memcpy(new_node._value, "\0", 1);
	}

	args_list.add(new_node);
}

bool argparser::is_arg(const char* arg) {
	list<arg_node>::node* found_arg = args_list.find<char*>([](char* a, list<arg_node>::node* n) {
		return strcmp(a, n->data._name) == 0;
	}, (char*) arg);

	if (found_arg == nullptr) {
		return false;
	} else {
		return true;
	}
}

char* argparser::get_arg(const char* arg) {
	list<arg_node>::node* found_arg = args_list.find<char*>([](char* a, list<arg_node>::node* n) {
		if (strcmp(a, n->data._name) == 0 && !n->data.used) {
			n->data.used = true;
			return true;
		} else {
			return false;
		}
	}, (char*) arg);

	if (found_arg == nullptr) {
		return nullptr;
	} else {
		if (found_arg->data._value[0] == '\0') {
			return nullptr;
		} else {
			return found_arg->data._value;
		}
	}
}

argparser* global_argparser;

void setup_global_argparser() {
	debugf("Setting up global argparser...\n");

	global_argparser = new argparser(boot::boot_info.command_line);
}