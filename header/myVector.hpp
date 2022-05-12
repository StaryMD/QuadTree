#pragma once

#include <cstdlib>
#include <string.h>
#include <algorithm>

template <typename T>
class myVector {
	T *vector;
	int size_;

public:

	myVector() {
		vector = (T *) malloc(size_ = 0);
	}

	~myVector() {
		free(vector);
	}

	inline T *begin() {
		return vector;
	}

	inline T *end() {
		return vector + size_;
	}

	inline T &at(int idx) {
		return *(vector + idx);
	}

	inline const int size() {
		return size_;
	}

	inline void push_back(const T &obj) {
		vector = (T *) realloc(vector, sizeof(T) * ++size_);
		*(vector + size_ - 1) = obj;
	}

	inline void erase(T *ptr) {
		memmove(ptr, ptr + 1, sizeof(T) * (--size_ - (ptr - vector)));
		//vector = (T *) realloc(vector, sizeof(T) * size_);
	}

	inline void clear() {
		vector = (T *) realloc(vector, size_ = 0);
	}

	void sort(bool (*cmp)(T &a, T &b)) {
		std::sort(vector, vector + size_, cmp);
	}

};
