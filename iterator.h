//
// Created by polarnight on 24-7-28, 下午6:39.
//

#ifndef TINYSTL__ITERATOR_H_
#define TINYSTL__ITERATOR_H_

/* <iterator.h> 用于 iterator 设计，
 * 还有涉及一些 iterator adapters 的实现（暂未实现），例如：
 *  insert iterator
 *  reverse iterator
 *  iostream iterator */

#include <cstddef> // for ptrdiff_t

#include "type_traits.h"

namespace tinystl {
/* 五种 iterator 类型 */
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

/* iterator 模板 */
template<typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T *, typename Reference = T &>
struct iterator {
  using iterator_category = Category;
  using value_type = T;
  using difference_type = Distance;
  using pointer = Pointer;
  using reference = Reference;
};

/* traits */
template<typename Iterator>
struct iterator_traits {
  using iterator_category = typename Iterator::iterator_category;
  using value_type = typename Iterator::value_type;
  using difference_type = typename Iterator::difference_type;
  using pointer = typename Iterator::pointer;
  using reference = typename Iterator::reference;
};

/* 针对 native pointer 设计的 traits template partial specialization */
template<typename T>
struct iterator_traits<T *> {
  using iterator_category = random_access_iterator_tag;
  using value_type = T;
  using difference_type = ptrdiff_t;
  using pointer = T *;
  using reference = T &;
};

/* 针对原生的 pointer-to-const 设计的 traits template partial specialization */
template<typename T>
struct iterator_traits<const T *> {
  using iterator_category = random_access_iterator_tag;
  using value_type = T;
  using difference_type = ptrdiff_t;
  using pointer = T *;
  using reference = T &;
};

/* 萃取 iterator 的 category */
template<typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator &) {
  using category = typename iterator_traits<Iterator>::iterator_category;
  return category();
}

/* 萃取 iterator 的 difference_type */
template<typename Iterator>
inline typename iterator_traits<Iterator>::difference_type *
difference_type(const Iterator &) {
  return static_cast<typename iterator_traits<Iterator>::difference_type *>(0);
}

/* 萃取 iterator 的 value_type */
template<typename Iterator>
inline typename iterator_traits<Iterator>::value_type *
value_type(const Iterator &) {
  return static_cast<typename iterator_traits<Iterator>::value_type *>(0);
}

/* distance() 系列函数
 * 用于计算迭代器间的距离 */

/* distance 的 input_iterator_tag 的版本 */
template<typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance_aux(InputIterator first, InputIterator last, input_iterator_tag) {
  typename iterator_traits<InputIterator>::difference_type n = 0;
  for (; first != last; ++first, ++n);
  return n;
}

/* distance 的 random_access_iterator_tag 的版本 */
template<typename RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
distance_aux(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
  return last - first;
}

template<typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
  return distance_aux(first, last, iterator_category(first));
}

template<typename InputIterator, typename Distance>
inline void distance_aux(InputIterator first, InputIterator last,
						 Distance &n, input_iterator_tag) {
  for (; first != last; ++first, ++n);
}

template<typename RandomAccessIterator, typename Distance>
inline void distance_aux(RandomAccessIterator first,
						 RandomAccessIterator last,
						 Distance &n,
						 random_access_iterator_tag) {
  n += last - first;
}

template<typename InputIterator, typename Distance>
inline void distance(InputIterator first, InputIterator last, Distance &n) {
  distance_aux(first, last, n, iterator_category(first));
}

/* advance() 系列函数
 * 用于让迭代器前进 n 个距离 */

/* advance 的 input_iterator_tag 的版本 */
template<typename InputIterator, typename Distance>
inline void advance_aux(InputIterator iter, const Distance &n, input_iterator_tag) {
  for (; n != 0; --n, ++iter);
}

/* advance 的 bidirectional_iterator_tag 的版本 */
template<typename BidirectionalIterator, typename Distance>
inline void advance_aux(BidirectionalIterator iter, const Distance &n, bidirectional_iterator_tag) {
  if (n >= 0)
	for (; n != 0; --n, ++iter);
  else
	for (; n != 0; ++n, --iter);
}

/* advance 的 random_access_iterator_tag 的版本 */
template<typename RandomAccessIterator, typename Distance>
inline void advance_aux(RandomAccessIterator iter, const Distance &n, random_access_iterator_tag) {
  iter += n;
}

template<typename InputIterator, typename Distance>
inline void advance(InputIterator iter, const Distance &n) {
  advance_aux(iter, n, iterator_category(iter));
}

/*****************************************************************************************/

/* reverse_iterator 模板 */

template<typename Iterator>
class reverse_iterator {
 private:
  Iterator current;

 public:
  using iterator_category = typename Iterator::iterator_category;
  using value_type = typename Iterator::value_type;
  using difference_type = typename Iterator::difference_type;
  using pointer = typename Iterator::pointer;
  using reference = typename Iterator::reference;

  using iterator_type = Iterator;
  using self = reverse_iterator<Iterator>;

 public:
  reverse_iterator() = default;
  explicit reverse_iterator(iterator_type i) : current(i) {}
  reverse_iterator(const self &rhs) : current(rhs.current) {}

 public:
  iterator_type base() const { return current; }
  reference operator*() const { // 实际对应正向迭代器的前一个位置
	auto tmp = current;
	return *--tmp;
  }
  pointer operator->() const { return &(operator*()); }
  self &operator++() { // 前进(++)变为后退(--)
	--current;
	return *this;
  }
  self operator++(int) {
	self tmp = *this;
	--current;
	return tmp;
  }
  self &operator--() { // 后退(--)变为前进(++)
	++current;
	return *this;
  }
  self operator--(int) {
	self tmp = *this;
	++current;
	return tmp;
  }
  self &operator+=(difference_type n) {
	current -= n;
	return *this;
  }
  self operator+(difference_type n) const { return self(current - n); }
  self &operator-=(difference_type n) {
	current += n;
	return *this;
  }
  self operator-(difference_type n) const { return self(current + n); }
  reference operator[](difference_type n) const { return *(*this + n); }

};

template<typename Iterator>
inline typename reverse_iterator<Iterator>::difference_type
operator-(const reverse_iterator<Iterator> &lhs,
		  const reverse_iterator<Iterator> &rhs) { return rhs.base() - lhs.base(); }

template<typename Iterator>
inline bool operator==(const reverse_iterator<Iterator> &lhs,
					   const reverse_iterator<Iterator> &rhs) { return lhs.base() == rhs.base(); }

template<typename Iterator>
inline bool operator<(const reverse_iterator<Iterator> &lhs,
					  const reverse_iterator<Iterator> &rhs) { return rhs.base() < lhs.base(); }

template<typename Iterator>
inline bool operator!=(const reverse_iterator<Iterator> &lhs,
					   const reverse_iterator<Iterator> &rhs) { return !(lhs == rhs); }

template<typename Iterator>
inline bool operator>(const reverse_iterator<Iterator> &lhs,
					  const reverse_iterator<Iterator> &rhs) { return rhs < lhs; }

template<typename Iterator>
inline bool operator<=(const reverse_iterator<Iterator> &lhs,
					   const reverse_iterator<Iterator> &rhs) { return !(rhs < lhs); }

template<typename Iterator>
inline bool operator>=(const reverse_iterator<Iterator> &lhs,
					   const reverse_iterator<Iterator> &rhs) { return !(lhs < rhs); }

} // namespace tinystl

#endif //TINYSTL__ITERATOR_H_
