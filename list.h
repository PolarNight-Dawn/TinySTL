//
// Created by polarnight on 24-8-4, 下午4:18.
//

#ifndef TINYSTL__LIST_H_
#define TINYSTL__LIST_H_

#include <initializer_list>

#include "iterator.h"
#include "memory.h"

namespace tinystl {
template<typename T>
struct list_node {
  list_node<T> *prev;
  list_node<T> *next;
  T data;
};

template<typename T>
struct list_iterator : public iterator<bidirectional_iterator_tag, T> {
  using iterator = list_iterator<T>;
  using self = list_iterator<T>;

  using value_type = T;
  using difference_type = ptrdiff_t;
  using pointer = T *;
  using reference = T &;
  using size_type = size_t;
  using link_type = list_node<T> *;

  link_type node;

  list_iterator() = default;
  list_iterator(link_type x) : node(x) {}
  list_iterator(const iterator &rhs) : node(rhs.node) {}

  bool operator==(const self &x) const { return node == x.node; }
  bool operator!=(const self &x) const { return node != x.node; }
  reference operator*() const { return (*node).data; }
  pointer operator->() const { return &(operator*()); }
  self &operator++() {
	node = (*node).next;
	return *this;
  }
  self operator++(int) {
	self tmp = *this;
	++*this;
	return tmp;
  }
  self &operator--() {
	node = (*node).prev;
	return *this;
  }
  self operator--(int) {
	self tmp = *this;
	--*this;
	return tmp;
  }
};

template<typename T>
struct list_const_iterator : public iterator<bidirectional_iterator_tag, T> {
  using const_iterator = list_const_iterator<T>;
  using self = list_const_iterator<T>;

  using value_type = T;
  using difference_type = ptrdiff_t;
  using pointer = const T *;
  using reference = const T &;
  using size_type = size_t;
  using link_type = list_node<T> *;

  link_type node;

  list_const_iterator() = default;
  list_const_iterator(link_type x) : node(x) {}
  list_const_iterator(const const_iterator &rhs) : node(rhs.node) {}

  bool operator==(const self &x) const { return node == x.node; }
  bool operator!=(const self &x) const { return node != x.node; }
  reference operator*() const { return (*node).data; }
  pointer operator->() const { return &(operator*()); }
  self &operator++() {
	node = (*node).next;
	return *this;
  }
  self operator++(int) {
	self tmp = *this;
	++*this;
	return tmp;
  }
  self &operator--() {
	node = (*node).prev;
	return *this;
  }
  self operator--(int) {
	self tmp = *this;
	--*this;
	return tmp;
  }
};

template<typename T, typename Allocator = Alloc>
class list {
 protected:
  using list_node_allocator = alloc<list_node<T>, Alloc>;

 public:
  using link_type = list_node<T> *;
  using iterator = list_iterator<T>;
  using const_iterator = list_const_iterator<T>;
  using reverse_iterator = tinystl::reverse_iterator<iterator>;
  using const_reverse_iterator = tinystl::reverse_iterator<const_iterator>;

  using iterator_categry = bidirectional_iterator_tag;
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

 protected:
  link_type node;
  link_type get_node() { return list_node_allocator::allocate(); }
  void put_node(link_type p) { list_node_allocator::deallocate(p); }
  link_type create_node(const_reference x);
  void destroy_node(link_type p);
  void empty_init();
  void fill_init(size_type n, const_reference value);
  template<typename InputIterator>
  void range_init(InputIterator first, InputIterator last);
  void transfer(iterator position, iterator first, iterator last);

 public:
  list() { empty_init(); }
  list(size_type n, const value_type &value) { fill_init(n, value); }
  explicit list(size_type n) { fill_init(n, value_type()); }
  template<typename InputIterator>
  list(InputIterator first, InputIterator last) { range_init(first, last); }
  list(std::initializer_list<T> rhs) {
	for (const T &value : rhs) insert(end(), value);
  }
  ~list() {
	clear();
	put_node(node);
  }

  list &operator=(const list &rhs) noexcept;
  list &operator=(std::initializer_list<T> rhs) noexcept;

  /* iterator 相关操作 */
  iterator begin() noexcept { return node->next; }
  const_iterator begin() const noexcept { return node->next; }
  iterator end() noexcept { return node; }
  const_iterator end() const noexcept { return node; }
  reverse_iterator rbegin() noexcept { return static_cast<reverse_iterator>(end()); }
  const_reverse_iterator rbegin() const noexcept { return static_cast<reverse_iterator>(end()); }
  reverse_iterator rend() noexcept { return static_cast<reverse_iterator>(begin()); }
  const_reverse_iterator rend() const noexcept { return static_cast<reverse_iterator>(begin()); }

  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }

  /* container 相关操作 */
  bool empty() const noexcept { return node->next == node; }
  size_type size() const noexcept { return distance(begin(), end()); }
  size_type max_size() const noexcept { return size_type(-1); }

  /* 取值相关操作 */
  reference front() { return node->next->data; }
  const_reference front() const { return node->next->data; }
  reference back() { return node->prev->data; }
  const_reference back() const { return node->prev->data; }
  reference operator[](const size_type &n);

  /* 修改链表操作 */
  void swap(list<T, Allocator> &rhs) { std::swap(node, rhs.node); }
  iterator insert(iterator pos, const T &value);
  iterator insert(iterator pos);
  template<typename InputIterator>
  void insert(iterator pos, InputIterator first, InputIterator last);
  void insert(iterator pos, size_type n, const T &value);
  void insert(iterator pos, int n, const T &value);
  void insert(iterator pos, long n, const T &value);
  void push_front(const T &value) { insert(begin(), value); }
  void push_back(const T &value) { insert(end(), value); }
  iterator erase(iterator pos);
  iterator erase(iterator first, iterator last);
  void resize(size_type new_size, const T &value);
  void resize(size_type new_size) { resize(new_size, T()); }
  void clear() { erase(begin(), end()); }
  void pop_front() { erase(begin()); }
  void pop_back() { erase(--end()); }
  void splice(iterator pos, list &x);
  void splice(iterator pos, list &, iterator i);
  void splice(iterator pos, list &, iterator first, iterator last);
  void remove(const T &value);
  void unique();
  void merge(list &x);
  void reverse();
  void sort();
};

template<typename T, typename Allocator>
typename list<T, Allocator>::link_type list<T, Allocator>::create_node(const_reference x) {
  link_type p = get_node();
  construct(&p->data, x);
  return p;
}

template<typename T, typename Allocator>
void list<T, Allocator>::destroy_node(link_type p) {
  destroy(&p->data);
  put_node(p);
}

template<typename T, typename Allocator>
void list<T, Allocator>::empty_init() {
  node = get_node();
  node->next = node;
  node->prev = node;
}

template<typename T, typename Allocator>
void list<T, Allocator>::fill_init(size_type n, const_reference value) {
  empty_init();
  try {
	insert(begin(), n, value);
  } catch (...) {
	clear();
	put_node(node);
  }
}

template<typename T, typename Allocator>
template<typename InputIterator>
void list<T, Allocator>::range_init(InputIterator first, InputIterator last) {
  empty_init();
  try {
	insert(begin(), first, last);
  } catch (...) {
	clear();
	put_node(node);
	throw;
  }
}

template<typename T, typename Allocator>
void list<T, Allocator>::transfer(iterator position, iterator first, iterator last) {
  if (position != last) {
	last.node->prev->next = position.node;
	first.node->prev->next = last.node;
	position.node->prev->next = first.node;
	link_type tmp = position.node->prev;
	position.node->prev = last.node->prev;
	last.node->prev = first.node->prev;
	first.node->prev = tmp;
  }
}

template<typename T, typename Allocator>
list<T, Allocator> &list<T, Allocator>::operator=(const list<T, Allocator> &rhs) noexcept {
  if (&rhs != this) {
	iterator first = begin();
	iterator last = end();
	const_iterator rhs_first = rhs.cbegin();
	const_iterator rhs_end = rhs.cend();

	for (; first != last && rhs_first != rhs_end; first++, rhs_first++)
	  *first = *rhs_first;
	if (first == last) insert(last, rhs_first, rhs_end);
	else erase(first, last);
  }
  return *this;
}

template<typename T, typename Allocator>
list<T, Allocator> &list<T, Allocator>::operator=(std::initializer_list<T> rhs) noexcept {
  if (&rhs != this) {
	iterator first = begin();
	iterator last = end();
	const_iterator rhs_first = rhs.begin();
	const_iterator rhs_end = rhs.end();

	for (; first != last && rhs_first != rhs_end; first++, rhs_first++)
	  *first = *rhs_first;
	if (first == last) insert(last, rhs_first, rhs_end);
	else erase(first, last);
  }
  return *this;
}

template<typename T, typename Allocator>
typename list<T, Allocator>::reference list<T, Allocator>::operator[](const size_type &n) {
  return *(begin() + n);
}

template<typename T, typename Allocator>
typename list<T, Allocator>::iterator list<T, Allocator>::insert(iterator pos,
																 const T &value) {
  link_type tmp = create_node(value);
  tmp->next = pos.node;
  tmp->prev = pos.node->prev;
  static_cast<link_type>(pos.node->prev)->next = tmp;
  pos.node->prev = tmp;
  return tmp;
}

template<typename T, typename Allocator>
template<typename InputIterator>
void list<T, Allocator>::insert(iterator pos,
								InputIterator first,
								InputIterator last) {
  for (; first != last; ++first, ++pos)
	pos = insert(pos, *first);
}

template<typename T, typename Allocator>
void list<T, Allocator>::insert(iterator pos, size_type n, const T &value) {
  for (; n > 0; --n, ++pos)
	pos = insert(pos, value);
}

template<typename T, typename Allocator>
void list<T, Allocator>::insert(iterator pos, int n, const T &value) {
  insert(pos, static_cast<size_type>(n), value);
}

template<typename T, typename Allocator>
void list<T, Allocator>::insert(iterator pos, long n, const T &value) {
  insert(pos, static_cast<size_type>(n), value);
}

template<typename T, typename Allocator>
typename list<T, Allocator>::iterator list<T, Allocator>::erase(iterator pos) {
  iterator tmp = pos;
  ++tmp;
  pos.node->prev->next = tmp.node;
  tmp.node->prev = pos.node->prev;
  destroy_node(pos.node);
  return tmp;
}

template<typename T, typename Allocator>
typename list<T, Allocator>::iterator list<T, Allocator>::erase(iterator first,
																iterator last) {
  while (first != last)
	erase(first++);
  return last;
}

template<typename T, typename Allocator>
void list<T, Allocator>::resize(size_type new_size, const T &value) {
  size_type old_size = size();
  if (old_size < new_size) {
	insert(end(), new_size - old_size, value);
  } else {
	iterator new_end = end();
	for (; old_size != new_size; --old_size, --new_end);
	erase(new_end, end());
  }
}

template<typename T, typename Allocator>
void list<T, Allocator>::splice(iterator pos, list<T, Allocator> &x) {
  if (!x.empty())
	transfer(pos, x.begin(), x.end());
}

template<typename T, typename Allocator>
void list<T, Allocator>::splice(iterator pos, list &, iterator i) {
  iterator j = i;
  ++j;
  if (pos == i || pos == j) return;
  transfer(pos, i, j);
}

template<typename T, typename Allocator>
void list<T, Allocator>::splice(iterator pos,
								list &,
								iterator first,
								iterator last) {
  if (first == last)
	return;
  transfer(pos, first, last);
}

template<typename T, typename Allocator>
void list<T, Allocator>::remove(const T &value) {
  iterator first = begin();
  iterator last = end();
  for (; first != last; ++first)
	if (*first == value)
	  erase(first);
}

template<typename T, typename Allocator>
void list<T, Allocator>::unique() {
  if (empty())
	return;
  iterator first = begin();
  iterator last = end();
  iterator next = first;
  while (++next != last) {
	if (*first == *next)
	  first = erase(first);
	else
	  first = next;
  }
}

template<typename T, typename Allocator>
void list<T, Allocator>::merge(list<T, Allocator> &x) {
  if (x.empty())
	return;
  iterator first1 = begin();
  iterator last1 = end();
  iterator first2 = x.begin();
  iterator last2 = x.end();
  while (first1 != last1 && first2 != last2) {
	if (*first2 < *first1) {
	  splice(first1, x, first2);
	  first2 = x.begin();
	} else
	  ++first1;
  }
  if (first1 == last1)
	splice(last1, x, first2, last2);
}

template<typename T, typename Allocator>
void list<T, Allocator>::reverse() {
  if (node->next == node || node->next->next == node)
	return;
  iterator first = begin();
  iterator last = end();
  ++first;
  while (first != last) {
	iterator old = first;
	++first;
	transfer(begin(), old, first);
  }
}

template<typename T, typename Allocator>
void list<T, Allocator>::sort() {
  if (node->next == node || node->next->next == node)
	return;
  list<T, Allocator> carry;
  list<T, Allocator> counter[64];
  int fill = 0;
  while (!empty()) {
	carry.splice(carry.begin(), *this, begin());
	int i = 0;
	while (i < fill && !counter[i].empty()) {
	  counter[i].merge(carry);
	  carry.swap(counter[i++]);
	}
	carry.swap(counter[i]);
	if (i == fill)
	  ++fill;
  }
  for (int i = 1; i < fill; ++i)
	counter[i].merge(counter[i - 1]);
  swap(counter[fill - 1]);
}

template<typename T, class Allocator>
inline bool operator==(const list<T, Allocator> &lhs, const list<T, Allocator> &rhs) {
  auto end1 = lhs.node;
  auto end2 = rhs.node;
  auto first1 = end1->next;
  auto first2 = end2->next;
  for (; first1 != end1 && first2 != end2;
		 first1 = first1->next, first2 = first2->next)
	if (first1->data != first2->data)
	  return false;
  return first1 == end1 && first2 == end2;
}

template<typename T, typename Allocator>
inline bool operator<(const list<T, Allocator> &lhs, const list<T, Allocator> &rhs) {
  auto end1 = lhs.node;
  auto end2 = rhs.node;
  auto first1 = end1->next;
  auto first2 = end2->next;
  for (; first1 != end1 && first2 != end2;
		 first1 = first1->next, first2 = first2->next) {
	if (first1->data < first2->date)
	  return true;
	else if (first2->data < first1->data)
	  return false;
  }
  return first1 != end1 && first2 == end2;
}

} // namespace tinystl
#endif //TINYSTL__LIST_H_
