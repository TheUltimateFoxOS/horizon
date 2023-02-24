#include <tmpfs_vfs.h>

#include <utils/assert.h>
#include <utils/string.h>

#include <memory/heap.h>

#include <utils/log.h>

using namespace fs;
using namespace fs::vfs;

tmpfs_mount::tmpfs_mount(char* name) {
	this->root_node = (tmpfs_dirent_t*) memory::malloc(sizeof(tmpfs_dirent));
	memset(this->root_node, 0, sizeof(tmpfs_dirent));

	this->root_node->type = TMPFS_DIR;
	strcpy(this->root_node->name, name);
}

tmpfs_mount::~tmpfs_mount() {
	//TODO: Free all the memory
}


file_t* tmpfs_mount::open(char* path) {
	debugf("Opening file %s\n", path);

	tmpfs_dirent_t* dirent = (tmpfs_dirent_t*) memory::malloc(sizeof(tmpfs_dirent));
	if (dirent == NULL) {
		debugf("Failed to allocate memory for dirent\n");
		return nullptr;
	}

	if (!this->get_dirent(path, dirent)) {
		memory::free(dirent);
		debugf("Failed to get dirent\n");
		return nullptr;
	}

	if (dirent->type == TMPFS_DIR) {
		memory::free(dirent);
		debugf("Is a dir\n");
		return nullptr;
	}

	file_t* file = (file_t*) memory::malloc(sizeof(file_t));
	if (file == NULL) {
		debugf("Failed to allocate memory for file\n");
		memory::free(dirent);
		return nullptr;
	}

	memset(file, 0, sizeof(file_t));

	file->mount = this;
	strcpy(file->buffer, path);
	file->size = dirent->size;
	file->data = dirent;

	return file;
}

void tmpfs_mount::close(file_t* file) {
	memory::free(file->data);
	memory::free(file);
}

void tmpfs_mount::read(file_t* file, void* buffer, size_t size, size_t offset) {
	tmpfs_dirent_t* dirent = (tmpfs_dirent_t*) file->data;
	this->read_dirent(dirent, buffer, size, offset);
}

void tmpfs_mount::write(file_t* file, void* buffer, size_t size, size_t offset) {
	tmpfs_dirent_t* dirent = (tmpfs_dirent_t*) file->data;
	this->write_dirent(dirent, buffer, size, offset);
}

void tmpfs_mount::touch(char* path) {
	debugf("Touching file %s\n", path);

	tmpfs_dirent_t* parent = (tmpfs_dirent_t*) memory::malloc(sizeof(tmpfs_dirent_t));
	if (parent == NULL) {
		debugf("Failed to allocate memory for dirent\n");
		return;
	}

	if (this->get_dirent(path, parent)) { // File already exists
		memory::free(parent);
		return;
	}

	if (!this->get_parent_dirent(path, parent)) {
		memory::free(parent);
		return;
	}

	this->create_diretnt(parent, strrchr(path, '/') + 1, TMPFS_FIL);

	memory::free(parent);
}

void tmpfs_mount::mkdir(char* path) {
	if (path[0] != '/') {
		return;
	}

	tmpfs_dirent_t* parent = (tmpfs_dirent_t*) memory::malloc(sizeof(tmpfs_dirent_t));
	if (parent == NULL) {
		debugf("Failed to allocate memory for dirent\n");
		return;
	}

	if (this->get_dirent(path, parent)) { // Directory already exists
		memory::free(parent);
		return;
	}

	if (!this->get_parent_dirent(path, parent)) {
		memory::free(parent);
		return;
	}

	this->create_diretnt(parent, strrchr(path, '/') + 1, TMPFS_DIR);

	memory::free(parent);
}

dir_t tmpfs_mount::dir_at(int idx, char* path) {
	if (path[0] != '/') {
		return dir_t {
			.is_none = true
		};
	}

	tmpfs_dirent_t* dirent = (tmpfs_dirent_t*) memory::malloc(sizeof(tmpfs_dirent_t));
	if (dirent == NULL) {
		debugf("Failed to allocate memory for dirent\n");
		return dir_t {
			.is_none = true
		};
	}

	if (!this->get_dirent(path, dirent)) {
		memory::free(dirent);
		return dir_t {
			.is_none = true
		};
	}

	if (dirent->type != TMPFS_DIR) {
		memory::free(dirent);
		return dir_t {
			.is_none = true
		};
	}

	if (idx * sizeof(tmpfs_dirent_t) >= dirent->size) {
		memory::free(dirent);
		return dir_t {
			.is_none = true
		};
	}

	tmpfs_dirent_t* child = (tmpfs_dirent_t*) memory::malloc(sizeof(tmpfs_dirent_t));
	if (child == NULL) {
		debugf("Failed to allocate memory for dirent\n");
		return dir_t {
			.is_none = true
		};
	}

	if (!this->readdir(dirent, idx, child)) {
		memory::free(dirent);
		memory::free(child);
		return dir_t {
			.is_none = true
		};
	}

	dir_t dir;
	strcpy(dir.name, child->name);
	dir.idx = idx;
	dir.is_none = false;
	dir.type = child->type == TMPFS_DIR ? ENTRY_DIR : ENTRY_FILE;

	memory::free(dirent);
	memory::free(child);

	return dir;
}

void tmpfs_mount::delete_(file_t* file) {
	
}

void tmpfs_mount::delete_dir(char* path) {
	if (path[0] != '/') {
		return;
	}

	
}

char* tmpfs_mount::name() {
	return this->root_node->name;
}

bool tmpfs_mount::read_dirent(tmpfs_dirent_t* dirent, void* buffer, size_t size, size_t offset) {
	if (offset + size > dirent->size) {
		return false;
	}

	if (dirent->data == NULL) {
		return false;
	}

	int data_id = offset / TMPFS_BLOCK_SIZE;
	int data_offset = offset % TMPFS_BLOCK_SIZE;

	int data_block_id = data_id / BLOCKS_PER_DATA_BLOCK;
	int data_block_offset = data_id % BLOCKS_PER_DATA_BLOCK;

	tmpfs_dirent_data_t* data = dirent->data;

	for (int i = 0; i < data_block_id; i++) {
		if (data->next == NULL) {
			return false;
		}

		data = data->next;
	}

	size_t read_data = 0;
	while (read_data != size) {
		if (data->data[data_block_offset] == NULL) {
			return false;
		}

		size_t to_read = size - read_data;
		if (to_read > TMPFS_BLOCK_SIZE - data_offset) {
			to_read = TMPFS_BLOCK_SIZE - data_offset;
		}

		memcpy((void*) ((uint64_t) buffer + read_data), (void*) ((uint64_t) data->data[data_block_offset] + data_offset), to_read);

		read_data += to_read;
		data_offset = 0;
		data_block_offset++;
		if (data_block_offset == BLOCKS_PER_DATA_BLOCK) {
			data_block_offset = 0;
			if (data->next == NULL) {
				return false;
			}

			data = data->next;
		}
	}

	memcpy(buffer, (void*) ((uint64_t) dirent->data + offset), size);

	return true;
}

void tmpfs_mount::write_dirent(tmpfs_dirent_t* dirent, void* buffer, size_t size, size_t offset) {
	if (offset + size > dirent->size) {
		dirent->size = offset + size;
	}

	if (dirent->data == NULL) {
		dirent->data = (tmpfs_dirent_data_t*) memory::malloc(sizeof(tmpfs_dirent_data_t));
		if (dirent->data == NULL) {
			debugf("Failed to allocate memory for dirent data\n");
			return;
		}

		memset(dirent->data, 0, sizeof(tmpfs_dirent_data_t));
	}

	int data_id = offset / TMPFS_BLOCK_SIZE;
	int data_offset = offset % TMPFS_BLOCK_SIZE;

	int data_block_id = data_id / BLOCKS_PER_DATA_BLOCK;
	int data_block_offset = data_id % BLOCKS_PER_DATA_BLOCK;

	tmpfs_dirent_data_t* data = dirent->data;

	for (int i = 0; i < data_block_id; i++) {
		if (data->next == NULL) {
			data->next = (tmpfs_dirent_data_t*) memory::malloc(sizeof(tmpfs_dirent_data_t));
			if (data->next == NULL) {
				debugf("Failed to allocate memory for dirent data\n");
				return;
			}

			memset(data->next, 0, sizeof(tmpfs_dirent_data_t));
		}

		data = data->next;
	}

	size_t written_data = 0;
	while (written_data != size) {
		if (data->data[data_block_offset] == NULL) {
			data->data[data_block_offset] = (tmpfs_block*) memory::malloc(TMPFS_BLOCK_SIZE);
			if (data->data[data_block_offset] == NULL) {
				debugf("Failed to allocate memory for dirent data\n");
				return;
			}

			memset(data->data[data_block_offset], 0, TMPFS_BLOCK_SIZE);
		}

		size_t to_write = size - written_data;
		if (to_write > TMPFS_BLOCK_SIZE - data_offset) {
			to_write = TMPFS_BLOCK_SIZE - data_offset;
		}

		memcpy((void*) ((uint64_t) data->data[data_block_offset] + data_offset), (void*) ((uint64_t) buffer + written_data), to_write);

		written_data += to_write;
		data_offset = 0;
		data_block_offset++;
		if (data_block_offset == BLOCKS_PER_DATA_BLOCK) {
			data_block_offset = 0;
			if (data->next == NULL) {
				data->next = (tmpfs_dirent_data_t*) memory::malloc(sizeof(tmpfs_dirent_data_t));
				if (data->next == NULL) {
					debugf("Failed to allocate memory for dirent data\n");
					return;
				}

				memset(data->next, 0, sizeof(tmpfs_dirent_data_t));
			}

			data = data->next;
		}
	}

	if (dirent->parent) { //The root dir has no parent
		this->writedir(dirent->parent, dirent); // Update the dirent entry in the parent directory
	}
}

bool tmpfs_mount::readdir(tmpfs_dirent_t* dir, int idx, tmpfs_dirent_t* buffer) {
	if (dir->type != TMPFS_DIR) {
		return false;
	}

	return this->read_dirent(dir, buffer, sizeof(tmpfs_dirent_t), idx * sizeof(tmpfs_dirent_t));
}

void tmpfs_mount::writedir(tmpfs_dirent_t* parent, tmpfs_dirent_t* dirent) {
	if (parent->type != TMPFS_DIR) {
		return;
	}

	tmpfs_dirent_t* buffer = (tmpfs_dirent_t*) memory::malloc(sizeof(tmpfs_dirent_t));
	if (buffer == NULL) {
		debugf("Failed to allocate memory for dirent\n");
		return;
	}

	int idx = 0;
	while (this->read_dirent(parent, buffer, sizeof(tmpfs_dirent_t), idx * sizeof(tmpfs_dirent_t))) {
		if (strcmp(buffer->name, dirent->name) == 0) {
			this->write_dirent(buffer, dirent, sizeof(tmpfs_dirent_t), idx * sizeof(tmpfs_dirent_t));

			memory::free(buffer);

			if (parent->parent) {
				this->writedir(buffer->parent, buffer);
			}
			return;
		}

		idx++;
	}

	memory::free(buffer);
}

void tmpfs_mount::create_diretnt(tmpfs_dirent_t* parent, char* name, int type) {
	tmpfs_dirent_t* dirent = (tmpfs_dirent_t*) memory::malloc(sizeof(tmpfs_dirent_t));
	if (dirent == NULL) {
		debugf("Failed to allocate memory for dirent\n");
		return;
	}
	memset(dirent, 0, sizeof(tmpfs_dirent_t));

	strcpy(dirent->name, name);
	dirent->type = type;
	dirent->parent = parent;

	this->write_dirent(parent, dirent, sizeof(tmpfs_dirent_t), parent->size);

	memory::free(dirent);
}

bool tmpfs_mount::get_dirent(char* path, tmpfs_dirent_t* buffer) {
	if (path[0] != '/') { //We only support absolute paths
		return false;
	}

	if (path[1] == 0) { //Root directory
		memcpy(buffer, this->root_node, sizeof(tmpfs_dirent_t));
		return true;
	}

	char* path_copy = (char*) memory::malloc(strlen(path));
	if (path_copy == NULL) {
		debugf("Failed to allocate memory for path copy\n");
		return false;
	}
	strcpy(path_copy, path + 1); //Skip the first slash

	char* tmp_path = path_copy;

	tmpfs_dirent_t* current_dirent = (tmpfs_dirent_t*) memory::malloc(sizeof(tmpfs_dirent_t));
	if (current_dirent == NULL) {
		debugf("Failed to allocate memory for dirent\n");
		return false;
	}
	memcpy(current_dirent, this->root_node, sizeof(tmpfs_dirent_t));

	char* next_slash = strchr(tmp_path, '/');
	while (true) {
		if (next_slash != NULL) {
			*next_slash = '\0';
		}

		tmpfs_dirent_t* tmp_dirent = (tmpfs_dirent_t*) memory::malloc(sizeof(tmpfs_dirent_t));
		if (tmp_dirent == NULL) {
			debugf("Failed to allocate memory for dirent\n");
			return false;
		}

		bool found = false;
		int idx = 0;
		while (this->readdir(current_dirent, idx, tmp_dirent)) {
			debugf("-c\n");
			if (strcmp(tmp_dirent->name, tmp_path) == 0) {
				memcpy(current_dirent, tmp_dirent, sizeof(tmpfs_dirent_t));
				found = true;
				break;
			}

			idx++;
		}

		memory::free(tmp_dirent);

		if (!found) {
			memory::free(path_copy);
			memory::free(current_dirent);
			return false;
		}

		if (next_slash != NULL) {
			tmp_path = next_slash + 1;
			next_slash = strchr(tmp_path, '/');
		} else {
			break;
		}
	}

	memcpy(buffer, current_dirent, sizeof(tmpfs_dirent_t));

	memory::free(path_copy);
	memory::free(current_dirent);

	return true;
}

bool tmpfs_mount::get_parent_dirent(char* path, tmpfs_dirent_t* buffer) {
	if (path[0] != '/') { //We only support absolute paths
		return false;
	}

	if (path[1] == '\0') { //The root directory has no parent
		return false;
	}

	int path_len = strlen(path);

	char* tmp_path = (char*) memory::malloc(path_len);
	if (tmp_path == NULL) {
		debugf("Failed to allocate memory for path copy\n");
		return false;
	}
	strcpy(tmp_path, path + 1); //Skip the first slash

	if (tmp_path[path_len - 1] == '/') {
		tmp_path[path_len - 1] = '\0';
	}

	char* last_slash = strrchr(tmp_path, '/');
	if (last_slash == NULL) { //The parent is the root directory since there is no other slash
		memory::free(tmp_path);
		memcpy(buffer, this->root_node, sizeof(tmpfs_dirent_t));
		return true;
	}

	*last_slash = '\0';

	bool out = this->get_dirent(tmp_path, buffer);

	memory::free(tmp_path);

	return out;
}