#pragma once

#include <utils/log.h>

template <typename T>
class queue {
	public:
		int size = 0;
		int len = 0;
		T* list = nullptr;

		queue(int initial_size) {
			size = initial_size;
			list = new T[size];
		}

		~queue() {
			delete[] list;
		}

		void add(T item) {
			if (len == size) {
				debugf("Queue is full, resizing\n");

				T* new_list = new T[size + 1];
				for (int i = 0; i < len; i++) {
					new_list[i] = list[i];
				}
				delete[] list;

				list = new_list;
				size++;
			}

			list[len++] = item;
		}

		T next() {
			this->list[this->len] = this->list[0];
			for (int i = 0; i < this->len; i++) {
				this->list[i] = this->list[i + 1];
			}

			return this->list[0];
		}

		void remove_first() {
			this->list[this->len] = this->list[0];

			for (int i = 0; i < this->len; i++) {
				this->list[i] = this->list[i + 1];
			}
			len--;
		}
};