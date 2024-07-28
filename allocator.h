//
// Created by polarnight on 24-3-26, 下午10:02.
//

#ifndef tinystl_ALLOCATOR_H
#define tinystl_ALLOCATOR_H

/* 这个头文件包含一个模板类 allocator
 * 仅仅是对于 ::operator new 和 ::operator delete 的简单封装
 * 并没有考虑任何效率上的优化 */

#include <cstddef> // for ptrdiff_t size_t
#include <algorithm> // for std::move
#include <climits> // for UINT_MAX

#include "construct.h"

namespace tinystl {
template<typename T>
class allocator {
 public:
  /* STL 要求的类型别名定义 */
  typedef T value_type; // using value_type = T;
  typedef T *pointer;
  typedef const T *const_pointer;
  typedef T &reference;
  typedef const T &const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

 public:
  /* STL 要求的类接口，使用静态函数实现使频繁调用下的开销减少 */
  /* 负责分配内存 */
  static T *allocate();
  static T *allocate(size_type n);

  /* 负责释放内存 */
  static void deallocate(T *ptr);
  static void deallocate(T *ptr, size_type n);

  /* 负责构造对象 */
  static void construct(T *ptr);
  static void construct(T *ptr, const T &value);
  static void construct(T *ptr, T &&value);
  template<class... Args>
  static void construct(T *ptr, Args &&...args);

  /* 负责析构对象 */
  static void destroy(T *ptr);
  static void destroy(T *first, T *last);

  /* 返回某个对象的地址。a.address(x) 等同于 &x */
  static T *address(T &val);

  /* 返回可成功配置的最大量 */
  static size_type max_size();

  /* 使 T 类型的 allocator 可以为 U 类型的对象分配内存 */
  template<typename U>
  struct rebind {
	typedef allocator<U> other;
  };
};

template<typename T>
T *allocator<T>::allocate() {
  return static_cast<T *>(::operator new(sizeof(T)));
}

template<typename T>
T *allocator<T>::allocate(size_type n) {
  if (n == 0) return nullptr;
  return static_cast<T *>(::operator new(n * sizeof(T)));
}

template<typename T>
void allocator<T>::deallocate(T *ptr) {
  if (ptr) ::operator delete(ptr);
}

template<typename T>
void allocator<T>::deallocate(T *ptr, size_type) {
  if (ptr) ::operator delete(ptr);
}

template<typename T>
void allocator<T>::construct(T *ptr) {
  tinystl::construct(ptr);
}

template<typename T>
void allocator<T>::construct(T *ptr, const T &value) {
  tinystl::construct(ptr, value);
}

template<typename T>
void allocator<T>::construct(T *ptr, T &&value) {
  tinystl::construct(ptr, std::move(value));
}

template<typename T>
template<typename ...Args>
void allocator<T>::construct(T *ptr, Args &&...args) {
  tinystl::construct(ptr, std::forward<Args>(args)...);
}

template<typename T>
void allocator<T>::destroy(T *ptr) {
  tinystl::destroy(ptr);
}

template<typename T>
void allocator<T>::destroy(T *first, T *last) {
  tinystl::destroy(first, last);
}

template<typename T>
T *allocator<T>::address(T &val) {
  return static_cast<T *>(&val);
}

template<typename T>
size_t allocator<T>::max_size() {
  return static_cast<size_t>(UINT_MAX / sizeof(T));
}

} // namespace tinystl

#endif //tinystl_ALLOCATOR_H
