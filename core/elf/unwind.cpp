#include <elf/unwind.h>

using namespace elf;

void elf::unwind(int max, uint64_t rbp, void (*callback)(int frame_num, uint64_t rip)) {
	stack_frame_t* stack = (stack_frame_t*) rbp;
	for(int i = 0; stack->rbp != 0 && i < max; i++) {
		callback(i, stack->rip);
		stack = stack->rbp;
	}
}