//
// Created by polarnight on 24-3-29, 上午1:07.
//

#ifndef TINYSTL_ALLOC_H
#define TINYSTL_ALLOC_H

#include <iostream>
#include <cstring>
#include <climits>

#include "construct.h"

/* <alloc.h> 的设计哲学如下：
 * 向 system heap 索要空间
 * 考虑内存不足时的应变措施
 * 考虑过多 “小型区块” 可能造成的 fragment
 * 未曾考虑 multi-threads 状态 */

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
  // 小型区块的上调界限
  enum { ALIGN = 8 };
  // 小型区块的上限
  enum { MAX_BYTES = 128 };
  // free-list 的个数
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
  static char *chunk_alloc(size_t size, size_t &nobjs);

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

/* 当 allocate() 发现 free list 没有可用区块时，为 free list 重新填充空间
 * n 已经上调至 8 的倍数 */
void *default_alloc::refill(size_t n) {
  size_t nobjs = 20;
  char *chunk = chunk_alloc(n, nobjs);

  // 如果只获取 1 个区块，该区块就直接分配给调用者
  if (nobjs == 1) return static_cast<void *>(chunk);
  // 否则就调整 free list 纳入新节点
  obj *volatile *my_free_list = free_list + freelist_index(n);

  // 在 chunk_alloc() 分配的空间内建立 free list
  void *result = static_cast<void *>(chunk); // 第 0 个区块返回给客端
  obj *curr_obj, *next_obj;
  *my_free_list = next_obj = (obj *)(chunk + n);
  for (int i = 1;; i++) {
	curr_obj = next_obj;
	next_obj = (obj *)((char *)next_obj + n);
	if (nobjs - 1 == i) {
	  curr_obj->next = nullptr;
	  break;
	} else {
	  curr_obj->next = next_obj;
	}
  }
  return result;
}

/* memory pool */
char *default_alloc::chunk_alloc(size_t size, size_t &nobjs) {
  char *result;
  size_t total_size = size * nobjs;
  size_t bytes_left = end_free - start_free;

  if (bytes_left >= total_size) {
	// 内存池剩余空间完全满足需求量，直接划拨
	result = start_free;
	start_free += total_size;
	return result;
  } else if (bytes_left >= size) {
	// 内存池剩余空间至少满足一个以上的区块，尽可能划拨
	nobjs = bytes_left / size;
	result = start_free;
	start_free += size * nobjs;
	return result;
  } else {
	// 内存池剩余空间连一个区块的大小都无法满足
	if (bytes_left > 0) {
	  // 将内存池剩余空间编入合适大小的 free-list
	  obj *volatile *my_free_list = free_list + freelist_index(bytes_left);
	  ((obj *)start_free)->next = *my_free_list;
	  *my_free_list = (obj *)start_free;
	}

	// 使用 malloc() 配置 heap 空间给内存池，大小为 当前需求 * 2 + 历史需求 / 16
	size_t bytes_to_get = 2 * total_size + round_up(heap_size >> 4);
	start_free = (char *)malloc(bytes_to_get);
	if (!start_free) {
	  // malloc() 失败，搜寻适当（未用区块，且区块够大）的 free list
	  obj *volatile *my_free_list, *ptr;
	  for (size_t i = size; i <= MAX_BYTES; i += ALIGN) {
		my_free_list = free_list + freelist_index(i);
		ptr = *my_free_list;
		if (ptr) {
		  *my_free_list = ptr->next;
		  start_free = (char *)ptr;
		  end_free = start_free + i;
		  return chunk_alloc(size, nobjs);
		}
	  }
	  // 注意，任何残余的零头内存空间都终将被编入适当的 free-list 中备用

	  // 到处都没有内存了，试着调用第一级配置器，看看 out-of-memory 机制是否起到一点作用
	  end_free = nullptr;
	  start_free = (char *)malloc_alloc::allocate(bytes_to_get);
	}
	heap_size += bytes_to_get;
	end_free = start_free + bytes_to_get;
	// malloc() 配置成功，递归调用自己向 refill() 划拨空间
	return chunk_alloc(size, nobjs);
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

#ifdef DIRECT_USE_MALLOC
using Alloc = default_alloc;
#else
using Alloc = default_alloc;
#endif

// SGI STL 特色分配器，需要一个模板参数，具有 STL 标准接口
template<typename T, typename Alloc>
class alloc {
 public:
  // STL 要求的类型别名定义
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

 public:
  // STL 要求的类接口，使用静态函数实现可以使频繁调用下减小开销
  static T *allocate();
  static T *allocate(size_type n);

  static void deallocate(T *ptr);
  static void deallocate(T *, size_type n);

  static void construct(T *ptr);
  static void construct(T *ptr, const T &value);
  static void construct(T *ptr, T &&value);

  static void destroy(T *ptr);
  static void destroy(T *first, T *last);

  static T *address(T &val);
  static size_t max_size();
  template<typename U>
  struct rebind {
	using other = alloc<U, Alloc>;
  };
};

template<typename T, typename Alloc>
T *alloc<T, Alloc>::allocate(size_t n) {
  return n == 0 ? 0 : static_cast<T *>(Alloc::allocate(n * sizeof(T)));
}

template<typename T, typename Alloc>
T *alloc<T, Alloc>::allocate() {
  return static_cast<T *>(Alloc::allocate(sizeof(T)));
}

template<typename T, typename Alloc>
void alloc<T, Alloc>::deallocate(T *ptr, size_t n) {
  if (n != 0)
	Alloc::deallocate((void *)ptr, n * sizeof(T));
}

template<typename T, typename Alloc>
void alloc<T, Alloc>::deallocate(T *ptr) {
  if (ptr)
	Alloc::deallocate((void *)ptr, sizeof(T));
}

template<typename T, typename Alloc>
void alloc<T, Alloc>::construct(T *ptr) {
  tinystl::construct(ptr);
}

template<typename T, typename Alloc>
void alloc<T, Alloc>::construct(T *ptr, const T &value) {
  tinystl::construct(ptr, value);
}

template<typename T, typename Alloc>
void alloc<T, Alloc>::construct(T *ptr, T &&value) {
  tinystl::construct(ptr, std::move(value));
}

template<typename T, typename Alloc>
void alloc<T, Alloc>::destroy(T *ptr) {
  tinystl::destroy(ptr);
}

template<typename T, typename Alloc>
void alloc<T, Alloc>::destroy(T *first, T *last) {
  tinystl::destroy(first, last);
}

template<typename T, typename Alloc>
T *alloc<T, Alloc>::address(T &val) {
  return (T *)(&val);
}

template<typename T, typename Alloc>
size_t alloc<T, Alloc>::max_size() {
  return (size_t)(UINT_MAX / sizeof(T));
}

} // namespace tinystl

#endif //TINYSTL_ALLOC_H
