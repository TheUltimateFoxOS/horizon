#include <utils/abort.h>

extern "C" {
	__attribute__((noreturn))
	void rust_oom() {
		abortf("RUST: out-of-memory");
	}
}