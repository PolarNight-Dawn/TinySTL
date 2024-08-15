//
// Created by polarnight on 24-7-31, 下午5:05.
//

#ifndef TINYSTL__UNINITIALIZED_H_
#define TINYSTL__UNINITIALIZED_H_

/* <uninitialized.h> 作用于未初始化空口上
 * 包含三个全局函数 uninitialized_copy(), uninitialized_fill(), uninitialized_fill_n()
 * 一一对应高层次函数 copy(), fill(), fill_n() */

#include <memory>
#include <cstring>

#include "iterator.h"
#include "construct.h"
#include "type_traits.h"

namespace tinystl {
/* uninitialized_copy() */
template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy_aux(InputIterator first,
											  InputIterator last,
											  ForwardIterator result,
											  _true_type) {
  return std::copy(first, last, result);
}

template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy_aux(InputIterator first,
											  InputIterator last,
											  ForwardIterator result,
											  _false_type) {
  ForwardIterator cur = result;
  try {
	for (; first != last; ++first, ++cur)
	  construct(&*cur, *first);
	return cur;
  } catch (...) {
	destroy(result, cur);
	throw;
  }
}

/*
template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
  using value_type = typename tinystl::iterator_traits<ForwardIterator>::value_type;
  using is_POD = typename type_traits<value_type>::is_POD_type;
  return uninitialized_copy_aux(first, last, result, is_POD());
}
*/

template<typename InputIterator, typename ForwardIterator, typename T>
inline ForwardIterator uninitialized_copy_POD(InputIterator first, InputIterator last, ForwardIterator result, T*) {
  using is_POD = typename type_traits<T>::is_POD_type;
  return uninitialized_copy_aux(first, last, result, is_POD());
}

template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
  return uninitialized_copy_POD(first, last, result, value_type(first));
}

inline char *uninitialized_copy(const char *first, const char *last, char *result) {
  memmove(result, first, last - first);
  return result + (last - first);
}

inline wchar_t *uninitialized_copy(const wchar_t *first, const wchar_t *last, wchar_t *result) {
  memmove(result, first, sizeof(wchar_t) * (last - first));
  return result + (last - first);
}

/* uninitialized_fill() */
template<typename ForwardIterator, typename T>
inline void uninitialized_fill_aux(ForwardIterator first,
								   ForwardIterator last,
								   const T &x,
								   _true_type) {
  return std::fill(first, last, x);
}

template<typename ForwardIterator, typename T>
inline void uninitialized_fill_aux(ForwardIterator first,
								   ForwardIterator last,
								   const T &x,
								   _false_type) {
  ForwardIterator cur = first;
  try {
	for (; cur != last; ++cur)
	  construct(&*cur, x);
	return cur;
  } catch (...) {
	destroy(first, cur);
	throw;
  }
}

/*
template<typename ForwardIterator, typename T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x) {
  using value_type = typename tinystl::iterator_traits<ForwardIterator>::value_type;
  using is_POD = typename type_traits<value_type>::is_POD_type;
  return uninitialized_fill_aux(first, last, x, is_POD());
}
*/

template<typename ForwardIterator, typename T, typename T1>
inline void uninitialized_fill_POD(ForwardIterator first, ForwardIterator last, const T &x, T1 *) {
  using is_POD = typename type_traits<T1>::is_POD_type;
  uninitialized_fill_aux(first, last, x, is_POD());
}

template<typename ForwardIterator, typename T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x) {
  uninitialized_fill_POD(first, last, x, value_type(first));
}

/* uninitialized_fill_n() */
template<typename ForwardIterator, typename Size, typename T>
inline ForwardIterator uninitialized_fill_n_aux(ForwardIterator first,
												Size n,
												const T &x,
												_true_type) {
  return std::fill_n(first, n, x);
}

template<typename ForwardIterator, typename Size, typename T>
inline ForwardIterator uninitialized_fill_n_aux(ForwardIterator first,
												Size n,
												const T &x,
												_false_type) {
  ForwardIterator cur = first;
  try {
	for (; n != 0; --n, ++cur)
	  construct(&*cur, x);
	return cur;
  } catch (...) {
	destroy(first, cur);
	throw;
  }
}

/*
template<typename ForwardIterator, typename Size, typename T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T &x) {
  using value_type = typename iterator_traits<ForwardIterator>::value_type;
  using is_POD = typename type_traits<value_type>::is_POD_type;
  return uninitialized_fill_n_aux(first, n, x, is_POD());
}
*/

template<typename ForwardIterator, typename Size, typename T, typename T1>
inline ForwardIterator uninitialized_fill_n_POD(ForwardIterator first, Size n, const T &x, T1 *) {
  using is_POD = typename type_traits<T1>::is_POD_type;
  return uninitialized_fill_n_aux(first, n, x, is_POD());
}

template<typename ForwardIterator, typename Size, typename T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T &x) {
  return uninitialized_fill_n_POD(first, n, x, value_type(first));
}

} // namespace tinystl

#endif //TINYSTL__UNINITIALIZED_H_
