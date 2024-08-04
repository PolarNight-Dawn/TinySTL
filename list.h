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
  using void_pointer = void *;
  void_pointer prev;
  void_pointer next;
  T data;
};

template<typename T, typename Ref, typename Ptr>
struct list_iterator {
  using iterator = list_iterator<T, T &, T *>;
  using self = list_iterator<T, Ref, Ptr>;

  using iterator_categry = bidirectional_iterator_tag;
  using value_type = T;
  using pointer = Ptr;
  using reference = Ref;
  using link_type = list_node<T> *;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

  link_type node;

  explicit list_iterator(link_type x) : node(x) {}
  list_iterator() = default;
  explicit list_iterator(const iterator &x) : node(x.node) {}

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
  using list_node = list_node<T>;
  using list_node_allocator = alloc<list_node, Alloc>;

 public:
  using link_type = list_node *;
  using iterator = list_iterator<T, T &, T *>;
  using const_iterator = const list_iterator<T, T &, T *>;

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

 public:
  list() { empty_init(); }
  list(size_type n, const_reference value) { fill_init(n, value); }
  explicit list(size_type n) { fill_init(n, T()); }
  template<typename InputIterator>
  list(InputIterator first, InputIterator last) { range_init(first, last); }
  ~list() {
	clear();
	put_node(node);
  }
  list<T, Allocator> &operator=(const list<T, Allocator> &rhs);
  list<T, Allocator> &operator=(std::initializer_list<T> rhs);

  /* iterator 相关操作 */
  iterator begin() noexcept { return node->next; }
  const_iterator begin() const noexcept { return node->next; }
  const_iterator cbegin() const noexcept { return begin(); }
  iterator end() noexcept { return node; }
  const_iterator end() const noexcept { return node; }
  const_iterator cend() const noexcept { return end(); }

  /* container 相关操作 */
  bool empty() const noexcept { return node->next == node; }
  size_type size() const noexcept { return distance(begin(), end()); }
  size_type max_size() const noexcept { return size_type(-1); }

  /* 取值相关操作 */
  reference front() { return node->next->data; }
  const_reference front() const { return node->next->data; }
  reference back() { return node->prev->data; }
  const_reference back() const { return node->prev->data; }
  reference operator[](const size_type& n);

  /* 修改链表操作 */
  void swap(list<T, Alloc>& rhs) { std::swap(node, rhs.node); }
  iterator insert(iterator pos, const T& value);
  iterator insert(iterator pos);
  template <typename InputIterator>
  void insert(iterator pos, InputIterator first, InputIterator last);
  void insert(iterator pos, size_type n, const T& value);
  void insert(iterator pos, int n, const T& value);
  void insert(iterator pos, long n, const T& value);
  void push_front(const T& value) { insert(begin(), value); }
  void push_back(const T& value) { insert(end(), value); }
  iterator erase(iterator pos);
  iterator erase(iterator first, iterator last);
  void resize(size_type new_size, const T& value);
  void resize(size_type new_size) { resize(new_size, T()); }
  void clear() { erase(begin(), end()); }
  void pop_front() { erase(begin()); }
  void pop_back() { erase(--end()); }
  void splice(iterator pos, list& x);
  void splice(iterator pos, list&, iterator i);
  void splice(iterator pos, list&, iterator first, iterator last);
  void remove(const T& value);
  void unique();
  void merge(list& x);
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
  }
}

template<typename T, typename Allocator>
list<T, Allocator> &list<T, Allocator>::operator=(const list<T, Allocator> &rhs) {
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
}

template<typename T, typename Allocator>
list<T, Allocator> &list<T, Allocator>::operator=(std::initializer_list<T> rhs) {
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
}

}
#endif //TINYSTL__LIST_H_
