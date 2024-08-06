//
// Created by polarnight on 24-8-5, 下午4:40.
//

#ifndef TINYSTL__DEQUE_H_
#define TINYSTL__DEQUE_H_

#include <initializer_list>

#include "memory.h"
#include "iterator.h"

namespace tinystl {
inline size_t deque_buf_size(size_t n, size_t sz) {
  return n != 0 ? n : (sz < 512 ? static_cast<size_t>(512 / sz) : static_cast<size_t>(1));
}

template<typename T, typename Ref, typename Ptr, size_t BufSize>
struct deque_iterator : public tinystl::iterator<random_access_iterator_tag, T> {
  using iterator = deque_iterator<T, T &, T *, BufSize>;
  using const_iterator = deque_iterator<T, const T &, const T *, BufSize>;
  using self = deque_iterator;

  using iterator_category = random_access_iterator_tag;
  using value_type = T;
  using pointer = Ptr;
  using reference = Ref;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using map_pointer = T **;

  T *cur; // iterator 所指缓冲区中的当前元素
  T *first; // iterator 所指缓冲区的头
  T *last; // iterator 所指缓冲区的尾
  map_pointer node; // 指向中控器 map 中的节点

  static size_t buffer_size() { return deque_buf_size(BufSize, sizeof(T)); }
  void set_node(map_pointer new_node) { // 切换到 map 中的下一个节点（也称缓冲区）
	node = new_node;
	first = *new_node;
	last = first + static_cast<difference_type>(buffer_size());
  }

  /* 构造函数 */
  deque_iterator(T *value_ptr, map_pointer node_ptr)
	  : cur(value_ptr), first(*node_ptr), last(*node_ptr + buffer_size()), node(node_ptr) {}
  deque_iterator() : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
  explicit deque_iterator(const iterator &iter) : cur(iter.cur), first(iter.first), last(iter.last), node(iter.node) {}

  /* 重载各种运算符 */
  reference operator*() const { return *cur; }
  pointer operator->() const { return &(operator*()); }
  difference_type operator-(const self &x) const {
	return static_cast<difference_type>(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
  }
  self &operator++() {
	++cur;
	if (cur == last) {
	  set_node(node + 1);
	  cur = first;
	}
	return *this;
  }
  self operator++(int) {
	self tmp = *this;
	++*this;
	return tmp;
  }
  self &operator--() {
	if (cur == first) {
	  set_node(node - 1);
	  cur = last;
	}
	--cur;
	return *this;
  }
  self operator--(int) {
	self tmp = *this;
	--*this;
	return tmp;
  }
  self &operator+=(difference_type n) {
	difference_type offset = n + (cur - first);
	if (offset >= 0 && offset < static_cast<difference_type>(buffer_size())) {
	  cur += n;
	} else {
	  difference_type node_offset = offset > 0 ? offset / static_cast<difference_type>(buffer_size()) :
									-static_cast<difference_type>((-offset - 1) / buffer_size()) - 1;
	  set_node(node + node_offset);
	  cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size()));
	}
	return *this;
  }
  self operator+(difference_type n) const {
	self tmp = *this;
	return tmp += n;
  }
  self &operator-=(difference_type n) { return *this += -n; }
  self operator-(difference_type n) const {
	self tmp = *this;
	return tmp -= n;
  }
  reference operator[](difference_type n) const { return *(*this + n); }
  bool operator==(const self &x) const { return cur == x.cur; }
  bool operator!=(const self &x) const { return cur != x.cur; }
  bool operator<(const self &x) const {
	return (node == x.node) ? (cur < x.cur) : (node < x.node);
  }
}; // deque_iterator end

template<typename T, typename Allocator = Alloc, size_t BufSize = 0>
class deque {
 public:
  using value_type = T;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using refenrence = T &;
  using const_refenrence = const T &;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

  using iterator = deque_iterator<T, T &, T *, BufSize>;
  using const_iterator = deque_iterator<T, const T &, const T *, BufSize>;

 protected:
  using map_pointer = pointer *;
  using data_allocator = alloc<value_type, Alloc>;
  using map_allocator = alloc<pointer, Alloc>;

  iterator start;
  iterator finish;
  map_pointer map;
  size_type map_size;

  static size_type buffer_size() { return deque_buf_size(0, sizeof(T)); }
  static size_type init_map_size() { return static_cast<size_type>(8); }
  void create_map_nodes(size_type num_element);
  void destroy_map_nodes();
  template<typename InputIterator>
  void copy_init(InputIterator first, InputIterator last);
  void fill_init(size_type n, const value_type &value);
  void insert_aux(iterator pos, size_type n, const value_type &value);
  pointer allocate_node() { return data_allocator::allocate(buffer_size()); }
  void deallocate_node(pointer ptr) { data_allocator::deallocate(ptr); }

 public:
  deque() { create_map_nodes(0); }
  deque(const deque &rhs) { copy_init(rhs.begin(), rhs.end()); }
  deque(size_type n, const value_type &value) { fill_init(n, value); }
  deque(int n, const value_type &value) { fill_init(n, value); }
  deque(long n, const value_type &value) { fill_init(n, value); }
  explicit deque(size_type n) { fill_init(n, value_type()); }
  template<typename InputIterator>
  deque(InputIterator first, InputIterator last) {
	copy_init(first, last);
  }
  ~deque() {
	destroy(start, finish);
	destroy_map_nodes();
  }

  deque &operator=(const deque &rhs);
  deque &operator=(std::initializer_list<T> rhs) {
	deque tmp(rhs);
	swap(tmp);
	return *this;
  }

  /* iterator 相关接口 */
  iterator begin() { return this->start; }
  const_iterator begin() const noexcept { return this->start; }
  iterator end() { return this->finish; }
  const_iterator end() const noexcept { return this->finish; }
  const_iterator cbegin() const noexcept { return this->start; }
  const_iterator cend() const noexcept { return this->finish; }

  /* capacity 相关操作 */
  size_type size() const { return end() - begin(); }
  size_type max_size() const { return static_cast<size_type>(-1); }
  bool empty() const { return begin() == end(); }

  /* 访问相关操作 */
  refenrence operator[](size_type n) { return start[static_cast<difference_type>(n)]; }
  const_refenrence operator[](size_type n) const { return start[static_cast<difference_type>(n)]; }
  refenrence front() { return *begin(); }
  const_refenrence front() const { return *begin(); }
  refenrence back() {
	iterator tmp = end();
	--tmp;
	return *tmp;
  }
  const_refenrence back() const {
	iterator tmp = end();
	--tmp;
	return *tmp;
  }

  /* container 相关操作 */
  void swap(deque &deq);
  void push_back(const value_type &value);
  void push_front(const value_type &value);
  void pop_back();
  void pop_front();
  iterator insert(iterator pos, const value_type &value);
  iterator insert(iterator pos) { return insert(pos, T()); }
  void insert(iterator pos, size_type n, const value_type &value);
  void insert(iterator pos, int n, const value_type &value) {
	insert(pos, size_type(n), value);
  }
  void insert(iterator pos, long n, const value_type &value) {
	insert(pos, size_type(n), value);
  }
  template<typename InputIterator>
  void insert(iterator pos, InputIterator first, InputIterator last);
  void resize(size_type new_size, const value_type &value);
  void resize(size_type new_size) { resize(new_size, T()); }
  iterator erase(iterator pos);
  iterator erase(iterator first, iterator last);
  void clear();

  /* 比较操作符的重载 */
  bool operator==(const deque<T> &rhs) {
	return size() == rhs.size() && std::equal(begin(), end(), rhs.begin());
  }
  bool operator!=(const deque<T> &rhs) { return !(*this == rhs); }
  bool operator<(const deque<T> &rhs) {
	return std::lexicographical_compare(begin(), end(), rhs.begin(),
										rhs.end());
  }
}; // deque end

template<typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_nodes(size_type num_element) {
  size_type num_nodes = num_element / buffer_size() + 1;
  map_size = std::max(init_map_size(), num_nodes + 2);
  map = map_allocator::allocate(map_size);
  map_pointer nstart = map + (map_size - num_nodes) / 2;
  map_pointer nfinish = nstart + num_nodes - 1;
  map_pointer cur;

  try {
	for (cur = nstart; cur <= nfinish; ++cur)
	  *cur = allocate_node();
  } catch (...) {
	for (map_pointer tmp = nstart; tmp < cur; ++tmp)
	  deallocate_node(*tmp);
	map_allocator::deallocate(map, map_size);
	throw;
  }

  start.set_node(nstart);
  finish.set_node(nfinish);
  start.cur = start.first;
  finish.cur = finish.first + num_element % buffer_size();
}

template<typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_map_nodes() {
  for (map_pointer cur = start.node; cur <= finish.node; ++cur)
	deallocate_node(*cur);
  map_allocator::deallocate(map, map_size);
}

template<typename T, typename Alloc, size_t BufSize>
template<typename InputIterator>
void deque<T, Alloc, BufSize>::copy_init(InputIterator first, InputIterator last) {

}

template<typename T, typename Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_init(size_type n, const value_type &value) {
  create_map_nodes(n);
  map_pointer cur;

  try {
	for (cur = start.node; cur < finish.node; ++cur)
	  uninitialized_fill(*cur, *cur + buffer_size(), value);
	uninitialized_fill(finish.first, finish, cur, value);
  } catch (...) {
	for (map_pointer mptr = start.node; mptr <= cur; ++mptr)
	  destroy(*mptr, *mptr + buffer_size());
	destroy_map_nodes();
	throw;
  }
}

template<typename T, typename Alloc, size_t BufSize>
deque<T, Alloc, BufSize> &deque<T, Alloc, BufSize>::operator=(const deque<T, Alloc, BufSize> &rhs) {
  const size_type len = size();
  if (&rhs != this) {
	if (len >= rhs.size())
	  erase(std::copy(rhs.begin(), rhs.end(), start), finish);
	else {
	  const_iterator mid = rhs.begin() + difference_type(len);
	  std::copy(rhs.begin(), rhs.end(), start);
	  insert(finish, mid, rhs.end());
	}
  }
  return *this;
}

}

#endif //TINYSTL__DEQUE_H_
