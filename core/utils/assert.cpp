#include <utils/assert.h>
#include <utils/abort.h>
#include <scheduler/scheduler.h>
#include <config.h>

void __assert_fail(const char* __assertion, const char* __file, unsigned int __line) {
	debugf("Assertion failed: \"%s\" in file %s at line %d\n", __assertion, __file, __line);

#ifdef SEND_SIGNALS
	if (!scheduler::handle_signal(scheduler::SIG_KASSERT_FAIL)) {
#endif
		abortf("Assertion failed: \"%s\" in file %s at line %d\n", __assertion, __file, __line);
#ifdef SEND_SIGNALS
	}
#endif
}