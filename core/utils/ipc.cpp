#include <utils/ipc.h>

#include <utils/string.h>
#include <utils/assert.h>
#include <utils/log.h>

namespace ipc {
	ipc_manager* global_ipc_manager;
};

using namespace ipc;

ipc_manager::ipc_manager() {
	memset(this->callbacks, 0, sizeof(this->callbacks));
	memset(this->ipc_names, 0, sizeof(this->ipc_names));
}

int ipc_manager::register_callback(char* name, ipc_callback_f callback) {
	int i;
	for (i = 0; this->ipc_names[i] != 0 && i < sizeof(this->ipc_names) / sizeof(this->ipc_names[0]); i++);

	assert(i < (sizeof(this->ipc_names) / sizeof(this->ipc_names[0])) - 1);

	this->callbacks[i] = callback;
	this->ipc_names[i] = name;

	debugf("Registered callback %s with hid %d\n", name, i);

	return i;
}

void ipc_manager::unregister_callback(int hid) {
	assert(hid < sizeof(this->ipc_names) / sizeof(this->ipc_names[0]));

	this->callbacks[hid] = 0;
	this->ipc_names[hid] = 0;

	debugf("Unregistered callback with hid %d\n", hid);
}

int ipc_manager::get_hid(char* name) {
	for (int i = 0; i < sizeof(this->ipc_names) / sizeof(this->ipc_names[0]); i++) {
		if (this->ipc_names[i] != 0) {
			if (strcmp(this->ipc_names[i], name) == 0) {
				return i;
			}
		}
	}

	return -1;
}

void ipc_manager::send_message(int hid, int func, void* data) {
	assert(hid < sizeof(this->ipc_names) / sizeof(this->ipc_names[0]));
	assert(this->callbacks[hid] != 0);

	this->callbacks[hid](func, data);
}

void ipc::init() {
	debugf("Initializing ipc manager...\n");
	global_ipc_manager = new ipc_manager();
}
