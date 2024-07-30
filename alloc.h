//
// Created by polarnight on 24-3-29, 上午1:07.
//

#ifndef TINYSTL_ALLOC_H
#define TINYSTL_ALLOC_H

#include <iostream>
#include <cstring>

#include "construct.h"

/* 具有 SGI 特色的两级分配器
 * 考虑内存不足时的应变措施
 * 考虑过多 “小型区块” 可能造成的 fragment
 * 排除了 multi-threads 状态 */

namespace tinystl {
/* 使用 malloc 和 free 实现的一级分配器
 * 可由客端设置 OOM 时的 out-of-memory handler */
class malloc_alloc {
 private:
  using FunPtr = void (*)();
 public:
  static void *allocate(size_t);
  static void deallocate(void *ptr);
  static void *reallocate(void *, size_t, size_t new_sz);
  static FunPtr set_malloc_handler(FunPtr f);

 private:
  static void *oom_malloc(size_t);
  static void *oom_realloc(void *, size_t);
  static void (*malloc_alloc_oom_handler)();
};

void *malloc_alloc::allocate(size_t n) {
  void *result = malloc(n);
  if (result == nullptr) result = malloc_alloc::oom_malloc(n);
  return result;
}

void malloc_alloc::deallocate(void *ptr) {
  free(ptr);
}

void *malloc_alloc::reallocate(void *ptr, size_t, size_t new_sz) {
  void *result = realloc(ptr, new_sz);
  if (result == nullptr) result = malloc_alloc::oom_realloc(ptr, new_sz);
  return result;
}

/* out-of-memory handler
 * 初值为 0，由客端设置 */
void (*malloc_alloc::malloc_alloc_oom_handler)() = nullptr;

/* 可以通过函数指针 f_ptr 指定自己的 out-of-memory handler
 * 返回 old 可以在需要时恢复到之前的 out-of-memory handler，
 * 以及可以在新的 out-of-memory handler 中调用旧的 out-of-memory handler，从而形成处理链 */
typename malloc_alloc::FunPtr malloc_alloc::set_malloc_handler(FunPtr f_ptr) {
  FunPtr old = malloc_alloc_oom_handler;
  malloc_alloc_oom_handler = f_ptr;
  return old;
}

void *malloc_alloc::oom_malloc(size_t n) {
  void (*my_malloc_handler)();
  void *result;

  for (;;) {
	my_malloc_handler = malloc_alloc_oom_handler;
	if (malloc_alloc_oom_handler == nullptr) {
	  std::cerr << "out of memory" << std::endl;
	  exit(1);
	}
	(*my_malloc_handler)();
	result = malloc(n);
	if (result) return result;
  }
}

void *malloc_alloc::oom_realloc(void *ptr, size_t new_sz) {
  void (*my_malloc_handler)();
  void *result;

  for (;;) {
	my_malloc_handler = malloc_alloc_oom_handler;
	if (malloc_alloc_oom_handler == nullptr) {
	  std::cerr << "out of memory" << std::endl;
	  exit(1);
	}
	(*my_malloc_handler)();
	result = realloc(ptr, new_sz);
	if (result) return result;
  }
}

/* 第二级配置器
 * 当区块过大超过 128 bytes 时，就移交第一级配置器处理，
 * 当区块小于 128 bytes 时，则以内存池（memory pool）管理（sub-allocation） */
class default_alloc {
 private:
  enum { ALIGN = 8 };
  enum { MAX_BYTES = 128 };
  enum { NFREELISTS = MAX_BYTES / ALIGN };

  union obj {
	union obj *next;
	char clinet_data[1];
  };

  static char *start_free;
  static char *end_free;
  static size_t heap_size;
  static obj *volatile free_list[NFREELISTS];

 private:
  static size_t round_up(size_t bytes);
  static size_t freelist_index(size_t bytes);
  static void *refill(size_t n);
  static char *chunk_alloc(size_t size, int &nobjs);

 public:
  static void *allocate(size_t n);
  static void deallocate(void *ptr, size_t n);
  static void *reallocate(void *ptr, size_t old_size, size_t new_size);
};

char *default_alloc::start_free = nullptr;
char *default_alloc::end_free = nullptr;
size_t default_alloc::heap_size = 0;
default_alloc::obj *volatile default_alloc::free_list[NFREELISTS] =
	{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	 nullptr, nullptr, nullptr, nullptr};

/* 将任何小额区块的内存需求量上调至 8 的倍数 */
inline size_t default_alloc::round_up(size_t bytes) {
  return ((bytes + ALIGN - 1) & ~(ALIGN - 1));
}

/* 根据区块的大小决定使用第 n 号 free-list， n 从 0 开始算 */
inline size_t default_alloc::freelist_index(size_t bytes) {
  return ((bytes + ALIGN - 1) / ALIGN - 1);
}

void *default_alloc::refill(size_t n) {
  int nobj = 20;
  char *chunk = chunk_alloc(n, nobj);

  if (nobj == 1) return static_cast<void *>(chunk);
  obj *volatile *my_free_list = free_list + freelist_index(n);
  void *result = static_cast<void *>(chunk);
  obj *curr_obj, *next_obj;
  *my_free_list = next_obj = (obj *)(chunk + n);
  for (int i = 1;; i++) {
	curr_obj = next_obj;
	next_obj = (obj *)((char *)next_obj + n);
	if (nobj - 1 == i) {
	  curr_obj->next = nullptr;
	  break;
	} else {
	  curr_obj->next = next_obj;
	}
  }
  return result;
}

char *default_alloc::chunk_alloc(size_t size, int &nobjs) {
  char *result;
  size_t total_size = size * nobjs;
  size_t bytes_left = end_free - start_free;

  if (bytes_left >= total_size) {
	result = start_free;
	start_free += total_size;
	return result;
  } else if (bytes_left >= size) {
	nobjs = bytes_left / size;
	result = start_free;
	start_free += size * nobjs;
	return result;
  } else {
	size_t bytes_to_get = 2 * total_size + round_up(heap_size >> 4);
	if (bytes_left > 0) {
	  obj *volatile *my_free_list = free_list + freelist_index(bytes_left);
	  ((obj *)start_free)->next = *my_free_list;
	  *my_free_list = (obj *)start_free;
	}
	start_free = (char *)malloc(bytes_to_get);
	if (start_free == nullptr) {
	  obj *volatile *my_free_list, *ptr;
	  for (int i = size; i<=MAX_BYTES; i+=ALIGN) {
		my_free_list = free_list + freelist_index(i);
		ptr = *my_free_list;
		if (ptr) {
		  *my_free_list = ptr->next;
		  start_free = (char*)ptr;
		  end_free = start_free + i;
		  return chunk_alloc(size, nobjs);
		}
		end_free = 0;
		start_free = (char*)malloc_alloc::allocate(bytes_to_get);
	  }
	  heap_size += bytes_to_get;
	  end_free = start_free + bytes_to_get;
	  return chunk_alloc(size, nobjs);
	}
  }
}

void *default_alloc::allocate(size_t n) {
  if (n > static_cast<size_t>(MAX_BYTES)) return malloc_alloc::allocate(n);
  obj *volatile *my_free_list = free_list + freelist_index(n);
  obj *result = *my_free_list;

  if (result == nullptr) {
	return refill(round_up(n));
  }
  *my_free_list = result->next;
  return static_cast<void *>(result);
}

void default_alloc::deallocate(void *ptr, size_t n) {
  if (n > static_cast<size_t>(MAX_BYTES)) return malloc_alloc::deallocate(ptr);

  obj *volatile *my_free_list = free_list + freelist_index(n);
  obj *obj_ptr = static_cast<obj *>(ptr);
  obj_ptr->next = *my_free_list;
  *my_free_list = obj_ptr;
}

void *default_alloc::reallocate(void *ptr, size_t old_size, size_t new_size) {
//   old_size 和 new_size 均超过 128 bytes，处于大区块;
  if (old_size > static_cast<size_t>(MAX_BYTES) && new_size > static_cast<size_t>(MAX_BYTES))
	return realloc(ptr,
				   new_size);

//   old_size 和 new_size 处于同一大小的小额区块
  if (round_up(old_size) == round_up(new_size)) return ptr;

//   old_size 处于小额区块，new_size超过 128 bytes，处于大区块
  void *result = realloc(nullptr, new_size);
  if (result) {
	memcpy(result, ptr, old_size);
	deallocate(ptr, old_size);
  }
  return result;
}

// SGI STL 特色分配器，需要一个模板参数，具有 STL 标准接口
template <typename T>
class alloc {
 public:
  // STL 要求的类型别名定义
  using value_type        = T;
  using pointer           = T*;
  using const_pointer     = const T*;
  using reference         = T&;
  using const_reference   = const T&;
  using size_type         = size_t;
  using difference_type   = ptrdiff_t;

 public:
  // STL 要求的类接口，使用静态函数实现可以使频繁调用下减小开销
  static T* allocate();
  static T* allocate(size_type n);

  static void deallocate(T* ptr);
  static void deallocate(T*, size_type n);

  static void construct(T* ptr);
  static void construct(T* ptr, const T& value);
  static void construct(T* ptr, T&& value);

  static void destroy(T* ptr);
  static void destroy(T* first, T* last);

  static T* address(T& val);
  static size_t max_size();
  template <typename U>
  struct rebind {
	using other = alloc<U>;
  };
};

template <typename T>
T* alloc<T>::allocate(size_t n) {
  return n == 0 ? 0 : static_cast<T*>(default_alloc::allocate(n * sizeof(T)));
}

template <typename T>
T* alloc<T>::allocate() {
  return static_cast<T*>(default_alloc::allocate(sizeof(T)));
}

template <typename T>
void alloc<T>::deallocate(T* ptr, size_t n) {
  if (n != 0)
	default_alloc::deallocate((void*)ptr, n * sizeof(T));
}

template <typename T>
void alloc<T>::deallocate(T* ptr) {
  if (ptr)
	default_alloc::deallocate((void*)ptr, sizeof(T));
}

template <typename T>
void alloc<T>::construct(T* ptr) {
  tinystl::construct(ptr);
}

template <typename T>
void alloc<T>::construct(T* ptr, const T& value) {
  tinystl::construct(ptr, value);
}

template <typename T>
void alloc<T>::construct(T* ptr, T&& value) {
  tinystl::construct(ptr, std::move(value));
}

template <typename T>
void alloc<T>::destroy(T* ptr) {
  tinystl::destroy(ptr);
}

template <typename T>
void alloc<T>::destroy(T* first, T* last) {
  tinystl::destroy(first, last);
}

template <typename T>
T* alloc<T>::address(T& val) {
  return (T*)(&val);
}

template <typename T>
size_t alloc<T>::max_size() {
  return (size_t)(WINT_MAX / sizeof(T));
}

} // namespace tinystl

#endif //TINYSTL_ALLOC_H
