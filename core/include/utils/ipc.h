#pragma once

namespace ipc {
	typedef void (*ipc_callback_f)(int func, void* data);

	class ipc_manager {
		public:
			ipc_manager();

			int register_callback(char* name, ipc_callback_f callback);
			void unregister_callback(int hid);

			int get_hid(char* name);

			void send_message(int hid, int func, void* data);

		private:
			ipc_callback_f callbacks[256];
			char* ipc_names[256];
	};

	void init();

	extern ipc_manager* global_ipc_manager;
}