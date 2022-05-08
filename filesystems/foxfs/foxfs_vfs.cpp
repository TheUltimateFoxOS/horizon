#include <foxfs_vfs.h>
#include <foxfs_fs.h>

#include <utils/assert.h>

#include <memory/heap.h>

#include <utils/log.h>

using namespace fs;
using namespace fs::vfs;

foxfs_mount::foxfs_mount(int disk_id, char* name) {
	this->fs = (foxfs_fs_t*) memory::malloc(sizeof(foxfs_fs_t));

	this->fs->ramfs = false;
	this->fs->disk_id = disk_id;
	this->fs->root_node = nullptr;

	//TODO: Make this work
}

foxfs_mount::foxfs_mount(char* name) {
	this->fs = (foxfs_fs_t*) memory::malloc(sizeof(foxfs_fs_t));

	this->fs->ramfs = true;
	this->fs->disk_id = -1;
	this->fs->root_node = (foxfs_dirent_t*) memory::malloc(sizeof(foxfs_dirent_t)); //Allocate the root node

	memset(this->fs->root_node, 0, sizeof(foxfs_dirent_t));
	strcpy(this->fs->root_node->name, "/");
	this->fs->root_node->file_type = FOXFS_DIR;
	this->fs->root_node->size = 0;

	this->fs->root_node->parent_address = 0; //The root node has no parent
	this->fs->root_node->parent_data_offset = 0;

	this->fs->root_node->data_address = 0; //By default, there is no data
}

foxfs_mount::~foxfs_mount() {
	if (this->fs->root_node != nullptr) {
		memory::free(this->fs->root_node);
	}
	memory::free(this->fs);
}


file_t* foxfs_mount::open(char* path) {
	debugf("Opening file %s\n", path);

	foxfs_dirent_t* node = (foxfs_dirent_t*) memory::malloc(sizeof(foxfs_dirent_t));
	int out = path_to_dirent(this->fs, path, node);
	if (out != 0) {
		memory::free(node);
		return nullptr;
	}
	if (node->file_type != FOXFS_FIL) { //Only open files
		memory::free(node);
		return nullptr;
	}

	file_t* file = new file_t;
	memset(file, 0, sizeof(file_t));

	file->mount = this;
	strcpy(file->buffer, path);
	file->size = node->size;
	file->data = (void*) node;

	return file;
}

void foxfs_mount::close(file_t* file) {
	debugf("Closing file\n");

	memory::free(file->data);
	delete file;
}

void foxfs_mount::read(file_t* file, void* buffer, size_t size, size_t offset) {
	debugf("Reading %d bytes from %d\n", size, offset);

	read_dirent(this->fs, (foxfs_dirent_t*) file->data, buffer, size, offset); //Read the data from the dirent's blocks
}

void foxfs_mount::write(file_t* file, void* buffer, size_t size, size_t offset) {
	debugf("Writing %d bytes to %d\n", size, offset);

	write_dirent(this->fs, (foxfs_dirent_t*) file->data, buffer, size, offset); //Write the data to the dirent's blocks
}

void foxfs_mount::touch(char* path) {
	create_dirent(this->fs, path, FOXFS_FIL);
}

void foxfs_mount::mkdir(char* path) {
	create_dirent(this->fs, path, FOXFS_DIR);
}

dir_t foxfs_mount::dir_at(int idx, char* path) {
	char path_copy[257]; //Max is 256 chars + null terminator
    memset(path_copy, 0, 257);
	strcpy(path_copy, path);
	
	if (path_copy[0] == 0) {
		path_copy[0] = '/';
		path_copy[1] = 0;
	} else {
		int path_len = strlen(path_copy);
		if (path_len > 1) {
			if (path_copy[path_len - 1] == '/') {
				path_copy[path_len - 1] = '\0';
			}
		}
	}

	debugf("Getting dir at %d, %s\n", idx, path_copy);

	foxfs_dirent_t parent_node;
	int out = path_to_dirent(this->fs, path_copy, &parent_node);
	if (out != 0) {
		return {
			.is_none = true
		};
	}
	if (parent_node.file_type != FOXFS_DIR) { //Only list directories
		return {
			.is_none = true
		};
	}

	foxfs_dirent_t child_node;
	out = readdir(this->fs, &parent_node, idx, &child_node);
	if (out != 0) {
		return {
			.is_none = true
		};
	}

	dir_t dir;
	memset(&dir, 0, sizeof(dir_t));

	dir.idx = idx;
	dir.is_none = false;

	if (child_node.file_type == FOXFS_DIR) {
		dir.type = ENTRY_DIR;
	} else {
		dir.type = ENTRY_FILE;
	}

	strcpy(dir.name, child_node.name);

	return dir;
}

void foxfs_mount::delete_(file_t* file) {
	//TODO: Implement
	debugf("FoxFS: Not implemented\n");
}

void foxfs_mount::delete_dir(char* path) {
	//TODO: Implement
	debugf("FoxFS: Not implemented\n");
}
