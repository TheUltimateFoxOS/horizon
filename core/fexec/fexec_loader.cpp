#include <fexec/fexec_loader.h>
#include <fexec/fexec.h>

#include <fs/vfs.h>

#include <memory/page_frame_allocator.h>

using namespace fexec;

scheduler::task_t* fexec::load_fexec(char* filename, const char** argv, const char** envp) {
    fs::vfs::file_t* file = fs::vfs::global_vfs->open(filename);
	if (file == nullptr) {
		return nullptr;
	}

	int page_amount = file->size / 0x1000 + 1;
    void* exec_contents = memory::global_allocator.request_pages(page_amount);
    
	fs::vfs::global_vfs->read(file, exec_contents, file->size, 0);

	scheduler::task_t* task = load_fexec(exec_contents, argv, envp);

	fs::vfs::global_vfs->close(file);

	memory::global_allocator.free_pages(exec_contents, page_amount);

	return task;
}

scheduler::task_t* fexec::load_fexec(void* ptr, const char** argv, const char** envp) {
    fexec_header_t* header = (fexec_header_t*) ptr;

    if (header->magic != fexec::MAGIC) {
        return nullptr;
    }
    if (header->version != 1) { //There is only a v1 for now
        return nullptr;
    }

    int page_amount = header->size / 0x1000 + 1;
    void* offset = memory::global_allocator.request_pages(page_amount);
    memcpy(offset, (void*) ((uint64_t) ptr + sizeof(fexec_header_t)), header->size);

    scheduler::task_t* new_task = scheduler::create_task((void*) ((uint64_t) offset + header->entry));
    new_task->is_binary = true;
    new_task->offset = offset;
    new_task->page_count = page_amount;
    new_task->argv = (char**) argv;
	new_task->envp = (char**) envp;

	return new_task;
}
