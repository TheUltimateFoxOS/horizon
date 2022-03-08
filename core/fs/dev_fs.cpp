#include <fs/dev_fs.h>

#include <utils/log.h>

using namespace fs;
using namespace fs::vfs;

namespace fs {
	dev_fs* global_devfs;
}

void dev_fs_file::read(file_t* file, void* buffer, size_t size, size_t offset) {
	debugf("dev_fs_file::read is not implemented\n");
}

void dev_fs_file::write(file_t* file, void* buffer, size_t size, size_t offset) {
	debugf("dev_fs_file::write is not implemented\n");
}

char* dev_fs_file::get_name() {
	debugf("dev_fs_file::get_name is not implemented\n");
	return (char*) "default";
}

dev_fs::dev_fs() : files(10) {

}

void dev_fs::register_file(dev_fs_file* file) {
	debugf("Adding dev fs file %s\n", file->get_name());

	files.add(file);
}
void dev_fs::unregister_file(dev_fs_file* file) {
	debugf("Removing dev fs file %s\n", file->get_name());

	list<dev_fs_file*>::node* file_node = files.find<dev_fs_file*>([] (dev_fs_file* f, list<dev_fs_file*>::node* n) {
		return f == n->data;
	}, file);

	if (file_node) {
		files.remove(file_node);
	} else {
		debugf("Could not find dev fs file %s\n", file->get_name());
	}
}

file_t* dev_fs::open(char* path) {
	debugf("Opening dev fs file %s\n", path);
	list<dev_fs_file*>::node* file_node = files.find<char*>([] (char* p, list<dev_fs_file*>::node* n) {
		return strcmp(p, n->data->get_name()) == 0;
	}, path);

	if (file_node) {
		file_t* file = new file_t;
		file->mount = this;
		file->size = -1;
		file->data = file_node->data;

		return file;
	} else {
		debugf("Could not find dev fs file %s\n", path);
		return nullptr;
	}
}

void dev_fs::close(file_t* file) {
	delete file;
}

void dev_fs::read(file_t* file, void* buffer, size_t size, size_t offset) {
	((dev_fs_file*) file->data)->read(file, buffer, size, offset);
}

void dev_fs::write(file_t* file, void* buffer, size_t size, size_t offset) {
	((dev_fs_file*) file->data)->write(file, buffer, size, offset);
}

dir_t dev_fs::dir_at(int idx, char* path) {
	list<dev_fs_file*>::node file = files.data[idx];

	if (!file.taken) {
		return {
			.is_none = true
		};
	}

	dir_t dir;
	memset(&dir, 0, sizeof(dir_t));

	dir.idx = idx;
	dir.is_none = false;
	dir.type = ENTRY_FILE;

	strcpy(dir.name, file.data->get_name());

	return dir;
}