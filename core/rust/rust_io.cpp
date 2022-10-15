#include <utils/log.h>
#include <utils/abort.h>

extern "C" {
	void __rust_print(const char* s) {
		printf(s);
	}

	void __rust_debug(const char* s) {
		debugf_raw(s);
	}

	void __rust_abort(const char* s) {
		abortf(s);
	}
}