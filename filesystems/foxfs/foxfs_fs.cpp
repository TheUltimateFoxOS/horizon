#include <foxfs_fs.h>

#include <memory/heap.h>

#include <utils/log.h>

using namespace fs;

int fs::read_disk_block(foxfs_fs_t* fs, uint32_t block, uint8_t* buffer) {
	driver::global_disk_manager->read(fs->disk_id, block * SECTORS_PER_BLOCK, SECTORS_PER_BLOCK, (void*) buffer);
    return 0;
}

int fs::write_disk_block(foxfs_fs_t* fs, uint32_t block, uint8_t* buffer) {
	driver::global_disk_manager->write(fs->disk_id, block * SECTORS_PER_BLOCK, SECTORS_PER_BLOCK, (void*) buffer);
    return 0;
}

uint64_t fs::create_data_block(foxfs_fs_t* fs) {
    if (fs->ramfs) {
        void* new_block = memory::malloc(BLOCK_SIZE);
        memset(new_block, 0, BLOCK_SIZE);
        return (uint64_t) new_block;
    } else {
        //TODO: Create a new block on disk
        debugf("FoxFS: Not implemented\n");
        return 0;
    }
}

int fs::read_dirent_data(foxfs_fs_t* fs, uint64_t data_address, void* buffer, size_t size, size_t offset) {
    if (fs->ramfs) {
        uint64_t current_block = data_address;
        if (current_block == 0) { //No data
            return 1;
        }

        uint32_t block_number = offset / BLOCK_DATA_SIZE; //Calculate the block number and the offset within the block
        uint32_t block_offset = offset % BLOCK_DATA_SIZE;
        uint32_t copied = 0;

        for (int i = 0; i < block_number; i++) { //Go to the correct block
            uint64_t next_block = ((foxfs_data_block_t*) current_block)->next;
            if (next_block == 0) {
                return 1;
            }
            current_block = next_block;
        }

        while (copied != size) { //Copy all the data
            uint32_t to_copy = BLOCK_DATA_SIZE - block_offset;
            if (to_copy > size - copied) {
                to_copy = size - copied;
            }

            memcpy((void*) (((uint64_t) buffer) + copied), (void*) (current_block + block_offset), to_copy);
            copied += to_copy;

            uint64_t next_block = ((foxfs_data_block_t*) current_block)->next;
            if (next_block == 0) { //No more data to read
                return 1;
            }
            current_block = next_block;
            block_offset = 0; //Reset the offset, it's only needed for the first block that we read
        }
    } else {
        //TODO: read from disk
        debugf("FoxFS: Not implemented\n");
        return 1;
    }

    return 0;
}

int fs::update_dirent_read(foxfs_fs_t* fs, foxfs_dirent_t* dirent) {
    if (dirent->parent_address == 0) { //If we are on the root node, then there is no parent.
        if (fs->ramfs) {
            memcpy(dirent, fs->root_node, sizeof(foxfs_dirent_t));
            return 0;
        } else {
            //TODO: write to disk
            debugf("FoxFS: Not implemented\n");
            return 1;
        }
    } else {
        return read_dirent_data(fs, dirent->parent_address, dirent, sizeof(foxfs_dirent_t), dirent->parent_data_offset);
    }
}

int fs::read_dirent(foxfs_fs_t* fs, foxfs_dirent_t* dirent, void* buffer, size_t size, size_t offset) {
    int out = update_dirent_read(fs, dirent);
    if (out != 0) {
        return out;
    }

    if (dirent->data_address == 0) { //No data
        return 1;
    }

    return read_dirent_data(fs, dirent->data_address, buffer, size, offset);
}

int fs::write_dirent_data(foxfs_fs_t* fs, uint64_t data_address, void* buffer, size_t size, size_t offset) {
    if (fs->ramfs) {
        uint64_t current_block = data_address;
        if (current_block == 0) { //No root data block, and we can't make one here.
            return 1;
        }

        uint32_t block_number = offset / BLOCK_DATA_SIZE; //Calculate the block number and the offset within the block
        uint32_t block_offset = offset % BLOCK_DATA_SIZE;
        uint32_t copied = 0;

        for (int i = 0; i < block_number; i++) { //Go to the correct block
            uint64_t next_block = ((foxfs_data_block_t*) current_block)->next;
            if (next_block == 0) {
                next_block = create_data_block(fs);
                if (next_block == 0) {
                    return 1;
                }
                ((foxfs_data_block_t*) current_block)->next = next_block;
            }
            current_block = next_block;
        }

        while (copied != size) { //Copy all the data
            uint32_t to_copy = BLOCK_DATA_SIZE - block_offset;
            if (to_copy > size - copied) {
                to_copy = size - copied;
            }

            memcpy((void*) (current_block + block_offset), (void*) (((uint64_t) buffer) + copied), to_copy);
            copied += to_copy;

            uint64_t next_block = ((foxfs_data_block_t*) current_block)->next;
            if (next_block == 0) {
                next_block = create_data_block(fs);
                if (next_block == 0) {
                    return 1;
                }
                ((foxfs_data_block_t*) current_block)->next = next_block;
            }
            current_block = next_block;
            block_offset = 0; //Reset the offset, it's only needed for the first block that we read
        }
    } else {
        //TODO: write to disk
        debugf("FoxFS: Not implemented\n");
    }

    return 0;
}

int fs::update_dirent_write(foxfs_fs_t* fs, foxfs_dirent_t* dirent) {
    if (dirent->parent_address == 0) { //If we are on the root node, then there is no parent.
        if (fs->ramfs) {
            memcpy(fs->root_node, dirent, sizeof(foxfs_dirent_t));
            return 0;
        } else {
            //TODO: write to disk
            debugf("FoxFS: Not implemented\n");
            return 1;
        }
    } else { //If we are on a subnode, then we need to update the parent's data block
        return write_dirent_data(fs, dirent->parent_address, dirent, sizeof(foxfs_dirent_t), dirent->parent_data_offset);
    }
}

int fs::write_dirent(foxfs_fs_t* fs, foxfs_dirent_t* dirent, void* buffer, size_t size, size_t offset) {
    bool modified_dirent = false;

    int out = update_dirent_read(fs, dirent);
    if (out != 0) {
        return out;
    }

    if (dirent->data_address == 0) { //If there is no data block, then we make one.
        dirent->data_address = create_data_block(fs);
        if (dirent->data_address == 0) {
            return 1;
        }

        modified_dirent = true;
    }

    out = write_dirent_data(fs, dirent->data_address, buffer, size, offset); //Write the data to the block
    if (out != 0) {
        return out;
    }

    if (offset + size > dirent->size) { //If the file got bigger, then update the info.
        dirent->size = offset + size;
        modified_dirent = true;
    }
    
    if (modified_dirent) { //If the dirent data was modified, then we update the dirent.
        update_dirent_write(fs, dirent);
    }

    return out;
}

int fs::readdir(foxfs_fs_t* fs, foxfs_dirent_t* dirent, int idx, foxfs_dirent_t* child_dir) {
    if (dirent->file_type != FOXFS_DIR) { //We can only read directories
        return 1;
    }

    int offset = sizeof(foxfs_dirent_t) * idx; //Calculate the offset of the dirent we want to read within it's parent
    if (dirent->size == 0 || offset >= dirent->size) {
        return 1;
    }

    memset(child_dir, 0, sizeof(foxfs_dirent_t));
    return read_dirent(fs, dirent, child_dir, sizeof(foxfs_dirent_t), offset);
}

int segment_path(char* path) { //Sets the first / to 0, and returns its position. Used in path_to_dirent
	int len = strlen(path);
	for (int i = 0; i < len; i++) {
		if (path[i] == '/') {
			path[i] = 0;
			return i + 1;
		}
	}

	return 0;
}

int fs::path_to_dirent(foxfs_fs_t* fs, char* path, foxfs_dirent_t* dirent) {
    if (path[0] != '/') { //We can only work with absolute paths
		return 1;
	}

    memcpy(dirent, fs->root_node, sizeof(foxfs_dirent_t)); //Start from the root node

	if (path[1] != 0) { //Don't search for root node, we have it
		char path_copy_alloc[257]; //Max is 256 chars + null terminator
        memset(path_copy_alloc, 0, 257);
        char* path_copy = path_copy_alloc;
		strcpy(path_copy, &path[1]); //Ignore the first /

		bool file_found = false;
		int next_segment = 0;
		do { //Split the path into segments
            path_copy = &path_copy[next_segment];
			next_segment = segment_path(path_copy);

            bool found_next = false;

            int idx = 0;
			foxfs_dirent_t tmp_node;
            while (readdir(fs, dirent, idx, &tmp_node) == 0) { //Find the next child node
                if (strcmp(tmp_node.name, path_copy) == 0) { //The name matches, move to the next segment
                    found_next = true;
                    memcpy(dirent, &tmp_node, sizeof(foxfs_dirent_t));
                    break;
                }

                idx++;
            }

            if (found_next) {
                if (next_segment == 0) { //We're at the end of the path, and all the names match, so we found the file
                    file_found = true;
                }
            } else {
                return 1;
            }
		} while (next_segment != 0);

		if (!file_found) {
			return 1;
		}
	}

    return 0;
}

int fs::create_dirent(foxfs_fs_t* fs, char* path, uint8_t file_type) {
    char path_copy[257]; //Max is 256 chars + null terminator
    memset(path_copy, 0, 257);
	strcpy(path_copy, path);

	int last_slash_idx = -1; //Get the parent dir
	for (int i = 0; i < strlen(path_copy); i++) {
		if (path_copy[i] == '/') {
			last_slash_idx = i;
		}
	}
	if (last_slash_idx == -1) {
		return 1;
	}
	path_copy[last_slash_idx] = '\0';

    int out = 0;
    foxfs_dirent_t parent_node;
    if (last_slash_idx == 0) { //The last slash is the root node, so we just use the root node
        out = path_to_dirent(fs, (char*) "/", &parent_node);
    } else {
        out = path_to_dirent(fs, path_copy, &parent_node);
    }
	if (out != 0) {
		return 1;
	}

    if (parent_node.file_type != FOXFS_DIR) { //We don't want to create a file in a file
        return 1;
    }

    char* child_name = &path_copy[last_slash_idx + 1];
    int child_len = strlen(child_name);
    if (child_len == 0) { //We don't want to create a dirent with no name
        return 1;
    }
    if (file_type == FOXFS_DIR && child_name[child_len - 1] == '/') { //Remove the trailing /
        child_name[child_len - 1] = 0;
    }
    for (int i = 0; i < child_len; i++) { //You can't have a file with a name that has a / in it
        if (child_name[i] == '/') {
            return 1;
        }
    }

    int idx = 0;
	foxfs_dirent_t tmp_node;
    while (readdir(fs, &parent_node, idx, &tmp_node) == 0) { //Check that there isn't already a child with the same name
        if (strcmp(tmp_node.name, child_name) == 0) {
            return 1;
        }

        idx++;
    }

	foxfs_dirent_t child_node;
	memset(&child_node, 0, sizeof(foxfs_dirent_t));

	strcpy(child_node.name, child_name);
	child_node.file_type = file_type;
	child_node.size = 0;

    child_node.parent_address = parent_node.data_address;
    child_node.parent_data_offset = parent_node.size;

	child_node.data_address = 0;

	out = write_dirent(fs, &parent_node, &child_node, sizeof(foxfs_dirent_t), parent_node.size);

    return  out;
}