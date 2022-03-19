#include <fs/saf.h>

#include <utils/log.h>
#include <utils/assert.h>

using namespace fs;
using namespace fs::vfs;

saf_mount::saf_mount(void* data) {
	this->data = data;
}

file_t* saf_mount::open(char* path) {
	debugf("open: %s\n", path);
	char* path_cpy = new char[strlen(path) + 1];
	strcpy(path_cpy, path);
	path_cpy[strlen(path)] = 0;;

	char** path_parts = process_path(path_cpy);

	saf_node_hdr_t* current_node = (saf_node_hdr_t*) data;
	saf_node_hdr_t* file = resolve(current_node, -1, path_parts);
	if (file == nullptr) {
		memory::free(path_parts);
		delete[] path_cpy;
		return nullptr;
	}

	if (file->flags == FLAG_ISFOLDER) {
		memory::free(path_parts);
		delete[] path_cpy;
		return nullptr;
	}

	saf_node_file_t* file_node = (saf_node_file_t*) file;

	file_t* f;

	f = new file_t;
	f->mount = this;
	f->size = file_node->size;
	f->data = (void*) ((uint64_t) data + file_node->addr);

	memory::free(path_parts);
	delete[] path_cpy;

	return f;
}

void saf_mount::close(file_t* file) {
	delete file;
}

void saf_mount::read(file_t* file, void* buffer, size_t size, size_t offset) {
	assert(file->size >= offset + size);

	memcpy(buffer, (void*) ((uint64_t) file->data + offset), size);
}


dir_t saf_mount::dir_at(int idx, char* path) {
	debugf("dir_at: %s (%d)\n", path, idx);
	char* path_cpy = new char[strlen(path) + 1];
	strcpy(path_cpy, path);
	path_cpy[strlen(path)] = 0;;

	char** path_parts = process_path(path_cpy);

	saf_node_hdr_t* current_node = (saf_node_hdr_t*) data;
	saf_node_hdr_t* folder;

	if (strcmp(path, (char*) "/") == 0 || strcmp(path, (char*) "") == 0) {
		folder = current_node;
	} else {
		folder = resolve(current_node, -1, path_parts);
	}

	if (folder == nullptr) {
		return {
			.is_none = true
		};
	}

	if (folder->flags != FLAG_ISFOLDER) {
		return {
			.is_none = true
		};
	}

	saf_node_folder_t* folder_node = (saf_node_folder_t*) folder;

	if (idx > folder_node->num_children - 1) {
		return {
			.is_none = true
		};
	} else {
		saf_node_hdr_t* child = (saf_node_hdr_t*) ((uint64_t) data + folder_node->children[idx]);

		dir_t dir;
		memset(&dir, 0, sizeof(dir));

		strcpy(dir.name, child->name);
		dir.idx = idx;
		dir.is_none = false;

		if (child->flags == FLAG_ISFOLDER) {
			dir.type = ENTRY_DIR;
		} else {
			dir.type = ENTRY_FILE;
		}

		return dir;
	}
}

saf_node_hdr_t* saf_mount::resolve(saf_node_hdr_t* curr, int level, char** path) {
	assert(curr->magic == MAGIC_NUMBER);

	// debugf("level %d (%s), curr->name %s\n", level,  level < 0 ? "-1" : path[level], curr->name);

	if (path[level + 1] == nullptr) {
		for (int i = 0; i < strlen(path[level]); i++) {
			if (path[level][i] >= 'A' && path[level][i] <= 'Z') {
				path[level][i] = path[level][i] + 32;
			}
		}

		char curr_name_cpy[512] = {0};
		strcpy(curr_name_cpy, curr->name);

		for (int i = 0; i < strlen(curr_name_cpy); i++) {
			if (curr_name_cpy[i] >= 'A' && curr_name_cpy[i] <= 'Z') {
				curr_name_cpy[i] = curr_name_cpy[i] + 32;
			}
		}

		
		if (strcmp(curr_name_cpy, path[level]) == 0) {
			return curr;
		} else {
			return nullptr;
		}
	} else {
		if (curr->flags != FLAG_ISFOLDER) {
			return nullptr;
		} else {

			saf_node_folder_t* folder = (saf_node_folder_t*) curr;
			for (int i = 0; i < folder->num_children; i++) {
				saf_node_hdr_t* child = (saf_node_hdr_t*) ((uint64_t) data + folder->children[i]);
				if (child->magic == MAGIC_NUMBER) {
					saf_node_hdr_t* result = resolve(child, level + 1, path);
					if (result != nullptr) {
						return result;
					}
				}
			}

			return nullptr;
		}
	}
}

char** saf_mount::process_path(char* path) {
	path++;
	char** path_parts = nullptr;
	int num_slashes = 0;
	int len = strlen(path);

	char* last_slash = path;
	for (int i = 0; i < len; i++) {
		if (path[i] == '/') {
			path[i] = '\0';
			path_parts = (char**) memory::realloc(path_parts, num_slashes * sizeof(char*), (num_slashes + 1) * sizeof(char*));
			path_parts[num_slashes] = last_slash;
			num_slashes++;

			last_slash = &path[i + 1];
		}
	}

	path_parts = (char**) memory::realloc(path_parts, num_slashes * sizeof(char*), (num_slashes + 2) * sizeof(char*));
	path_parts[num_slashes] = last_slash;
	path_parts[num_slashes + 1] = nullptr;

	return path_parts;
}
