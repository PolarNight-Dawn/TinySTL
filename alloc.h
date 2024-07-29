//
// Created by polarnight on 24-3-29, 上午1:07.
//

#ifndef TINYSTL_ALLOC_H
#define TINYSTL_ALLOC_H

#include <iostream>

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
}

#endif //TINYSTL_ALLOC_H
