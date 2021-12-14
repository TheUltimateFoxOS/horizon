#include <fs/vfs.h>

#include <utils/log.h>
#include <utils/string.h>

using namespace fs;
using namespace fs::vfs;

namespace fs {
	namespace vfs {
		vfs_manager* global_vfs;
	}
}

vfs_manager::vfs_manager() : mounts(10) {

}

file_t* vfs_manager::open(char* path) {
	char _filename[1024] = "";
	char* file_path = NULL;
	strcpy(_filename, path);
	int len = strlen(_filename);

	list<mount_store_t>::node* found_device = nullptr;

	for (int i = 0; i < len; i++) {
		if(_filename[i] == ':') {
			_filename[i] = 0;
			file_path = (char*) ((uint64_t) &_filename[i] + 1);

			found_device = mounts.find<char*>([](char* d, list<mount_store_t>::node* n) {
				return strcmp(d, n->data.name) == 0;
			}, _filename);

			break;
		}
	}

	if (found_device == nullptr) {
		debugf("No device found for path: %s\n", path);
		return nullptr;
	}

	return found_device->data.mount->open(file_path);
}

void vfs_manager::close(file_t* file) {
	file->mount->close(file);
}

void vfs_manager::read(file_t* file, void* buffer, size_t size, size_t offset) {
	file->mount->read(file, buffer, size, offset);
}

void vfs_manager::write(file_t* file, void* buffer, size_t size, size_t offset) {
	file->mount->write(file, buffer, size, offset);
}

void vfs_manager::delete_(file_t* file) {
	file->mount->delete_(file);
}

void vfs_manager::mkdir(char* path) {
	char _filename[1024] = "";
	char* file_path = NULL;
	strcpy(_filename, path);
	int len = strlen(_filename);

	list<mount_store_t>::node* found_device = nullptr;

	for (int i = 0; i < len; i++) {
		if(_filename[i] == ':') {
			_filename[i] = 0;
			file_path = (char*) ((uint64_t) &_filename[i] + 1);

			found_device = mounts.find<char*>([](char* d, list<mount_store_t>::node* n) {
				return strcmp(d, n->data.name) == 0;
			}, _filename);

			break;
		}
	}

	if (found_device == nullptr) {
		debugf("No device found for path: %s\n", path);
		return;
	}

	found_device->data.mount->mkdir(file_path);
}

dir_t vfs_manager::dir_at(int idx, char* path) {
	char _filename[1024] = "";
	char* file_path = NULL;
	strcpy(_filename, path);
	int len = strlen(_filename);

	list<mount_store_t>::node* found_device = nullptr;

	for (int i = 0; i < len; i++) {
		if(_filename[i] == ':') {
			_filename[i] = 0;
			file_path = (char*) ((uint64_t) &_filename[i] + 1);

			found_device = mounts.find<char*>([](char* d, list<mount_store_t>::node* n) {
				return strcmp(d, n->data.name) == 0;
			}, _filename);

			break;
		}
	}

	if (found_device == nullptr) {
		debugf("No device found for path: %s\n", path);

		return {
			.is_none = true
		};
	}

	return found_device->data.mount->dir_at(idx, file_path);
}

void vfs_manager::register_mount(char* device, vfs_mount* vfs_mount_point) {
	mount_store_t new_mount = {
		.mount = vfs_mount_point
	};

	strcpy(new_mount.name, device);

	mounts.add(new_mount);
}

void vfs_manager::unregister_mount(char* device) {
	list<mount_store_t>::node* found_device = mounts.find<char*>([](char* d, list<mount_store_t>::node* n) {
		return strcmp(d, n->data.name) == 0;
	}, device);

	if (found_device) {
		mounts.remove(found_device);
	}
}

file_t* vfs_mount::open(char* path) {
	debugf("WARNING: vfs_mount::open() not implemented\n");
	return nullptr;
}

void vfs_mount::close(file_t* file) {
	debugf("WARNING: vfs_mount::close() not implemented\n");
}

void vfs_mount::read(file_t* file, void* buffer, size_t size, size_t offset) {
	debugf("WARNING: vfs_mount::read() not implemented\n");
}

void vfs_mount::write(file_t* file, void* buffer, size_t size, size_t offset) {
	debugf("WARNING: vfs_mount::write() not implemented\n");
}

void vfs_mount::delete_(file_t* file) {
	debugf("WARNING: vfs_mount::delete() not implemented\n");
}

void vfs_mount::mkdir(char* path) {
	debugf("WARNING: vfs_mount::mkdir() not implemented\n");
}

dir_t vfs_mount::dir_at(int idx, char* path) {
	debugf("WARNING: vfs_mount::dir_at() not implemented\n");

	return {
		.is_none = true
	};
}

void fs::vfs::setup() {
	debugf("Creating global vfs...\n");
	global_vfs = new vfs_manager();
}