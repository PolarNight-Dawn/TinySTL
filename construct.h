//
// Created by polarnight on 24-3-25, 下午9:15.
//

#ifndef TINYSTL_CONSTRUCT_H
#define TINYSTL_CONSTRUCT_H

/* 这个头文件包含两个函数 construct() 和 destroy()
 * construct：负责对象的构造
 * destroy：负责对象的析构 */

#include <new> // for placement new
#include <utility> // for std::forward

#include "type_traits.h"
#include "iterator.h"

namespace tinystl {
/* construct 构造对象 */

template<typename T>
inline void construct(T *ptr) {
  ::new(static_cast<void *>(ptr)) T();
}

template<typename T1, typename T2>
inline void construct(T1 *ptr, const T2 &value) {
  ::new(static_cast<void *>(ptr)) T1(value);
}

template<typename T1, typename T2>
inline void construct(T1 *ptr, T2 &&value) {
  ::new(static_cast<void *>(ptr)) T1(std::forward<T2>(value));
}

template<typename T, typename ...Args>
inline void construct(T *ptr, Args &&...args) {
  new(static_cast<void *>(ptr)) T(std::forward<Args>(args)...);
}

/* destroy 析构对象 */

/* destroy 版本一 接受一个指针 */
template<typename T>
inline void destroy(T *ptr) {
  destroy_one(ptr, std::is_trivially_destructible<T>{});
}

/* value type 有 trivial destructor 不调用析构操作 */
template<typename T>
inline void destroy_one(T *, std::true_type) {}

/* value type 有 non-trivial destructor */
template<typename T>
inline void destroy_one(T *ptr, std::false_type) {
  if (ptr) ptr->~T();
}

/* destroy 版本二 接受两个迭代器
 * 判断 value type 是否有 trivial destructor */
template<typename ForwardIter>
void destroy(ForwardIter first, ForwardIter last) {
  typedef typename tinystl::iterator_traits<ForwardIterator>::value_type value_type;
  typedef typename type_traits<value_type>::has_trivially_destructible is_trivial_dtor;
  destroy_cat(first, last, is_trivial_dtor());
}

/* value type 有 trivial destructor 不调用析构操作 */
template<typename ForwardIter>
void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

/* value type 有 non-trivial destructor */
template<typename ForwardIter>
void destroy_cat(ForwardIter first, ForwardIter last, std::false_type) {
  for (; first < last; ++first)
	destroy(&*first);
}

}

#endif //TINYSTL_CONSTRUCT_H
