#include <fs/fd.h>

using namespace fs;
using namespace fs::vfs;

namespace fs {
	file_descriptor_manager* global_fd_manager;
}

file_descriptor::file_descriptor(int fd) {
	this->fd = fd;
	this->file = nullptr;
}

file_descriptor::~file_descriptor() {}

void file_descriptor::close() {
	if (this->file != nullptr) {
		global_vfs->close(this->file);
	}
}

void file_descriptor::open(char* path) {
	this->file = global_vfs->open(path);
}

void file_descriptor::read(void* buffer, size_t size, size_t offset) {
	if (this->file != nullptr) {
		global_vfs->read(this->file, buffer, size, offset);
	}
}

void file_descriptor::write(void* buffer, size_t size, size_t offset) {
	if (this->file != nullptr) {
		global_vfs->write(this->file, buffer, size, offset);
	}
}

void file_descriptor::delete_() {
	if (this->file != nullptr) {
		global_vfs->delete_(this->file);
	}
}


file_descriptor_manager::file_descriptor_manager() : fds(10) {
	this->curr_fd = 10;
}

int file_descriptor_manager::alloc_fd() {
	return ++this->curr_fd;
}

void file_descriptor_manager::free_fd(int fd) {
	list<file_descriptor*>::node* n = this->fds.find<int>([](int _fd, list<file_descriptor*>::node* n) {
		return _fd == n->data->fd;
	}, fd);

	if (!(n == nullptr)) {
		fds.remove(n);
	}
}

void file_descriptor_manager::register_fd(file_descriptor* fd_obj) {
	fds.add(fd_obj);
}

file_descriptor* file_descriptor_manager::get_fd(int fd) {
	list<file_descriptor*>::node* n = this->fds.find<int>([](int _fd, list<file_descriptor*>::node* n) {
		return _fd == n->data->fd;
	}, fd);

	if (!(n == nullptr)) {
		return n->data;
	} else {
		return nullptr;
	}
}