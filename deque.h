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
  using reference = T &;
  using const_reference = const T &;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

  using iterator = deque_iterator<T, T &, T *, BufSize>;
  using const_iterator = deque_iterator<T, const T &, const T *, BufSize>;
  using reverse_iterator = tinystl::reverse_iterator<iterator>;
  using const_reverse_iterator = tinystl::reverse_iterator<const_iterator>;

 protected:
  using map_pointer = pointer *;
  using data_allocator = alloc<value_type, Allocator>;
  using map_allocator = alloc<pointer, Allocator>;

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
  iterator reserve_elements_at_front(size_type n);
  iterator reserve_elements_at_back(size_type n);
  pointer allocate_node() { return data_allocator::allocate(buffer_size()); }
  void deallocate_node(pointer ptr) { data_allocator::deallocate(ptr); }
  void reserve_map_at_front(size_type nodes_to_add = 1) {
	if (nodes_to_add > map - start.node)
	  reallocate_map(nodes_to_add, true);
  }
  void reserve_map_at_back(size_type nodes_to_add = 1) {
	if (nodes_to_add + 1 > map_size - (finish.node - map))
	  reallocate_map(nodes_to_add, false);
  }
  void reallocate_map(size_type nodes_to_add, bool add_at_front);

 public:
  deque() { create_map_nodes(0); }
  deque(const deque &rhs) { copy_init(rhs.begin(), rhs.end()); }
  deque(size_type n, const value_type &value) { fill_init(n, value); }
  deque(int n, const value_type &value) { fill_init(n, value); }
  deque(long n, const value_type &value) { fill_init(n, value); }
  explicit deque(size_type n) { fill_init(n, value_type()); }
  template<typename InputIterator>
  deque(InputIterator first, InputIterator last) { copy_init(first, last); }
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

  reverse_iterator rbegin() noexcept { return reverse_iter(finish); }
  const_reverse_iterator rbgein() const noexcept { return const_reverse_iter(finish); }
  reverse_iterator rend() noexcept { return reverse_iter(start); }
  const_reverse_iterator rend() const noexcept { return const_reverse_iter(start); }

  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }

  /* capacity 相关操作 */
  size_type size() const { return end() - begin(); }
  size_type max_size() const { return static_cast<size_type>(-1); }
  bool empty() const { return begin() == end(); }

  /* 访问相关操作 */
  reference operator[](size_type n) { return start[static_cast<difference_type>(n)]; }
  const_reference operator[](size_type n) const { return start[static_cast<difference_type>(n)]; }
  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }
  reference back() {
	iterator tmp = end();
	--tmp;
	return *tmp;
  }
  const_reference back() const {
	iterator tmp = end();
	--tmp;
	return *tmp;
  }

  /* container 相关操作 */
  void swap(deque &rhs);
  void push_back(const value_type &value);
  void push_front(const value_type &value);
  void pop_back();
  void pop_front();
  iterator insert(iterator pos, const value_type &value);
  iterator insert(iterator pos) { return insert(pos, T()); }
  void insert(iterator pos, size_type n, const value_type &value);
  void insert(iterator pos, int n, const value_type &value) { insert(pos, size_type(n), value); }
  void insert(iterator pos, long n, const value_type &value) { insert(pos, size_type(n), value); }
  template<typename InputIterator>
  void insert(iterator pos, InputIterator first, InputIterator last);
  void resize(size_type new_size, const value_type &value);
  void resize(size_type new_size) { resize(new_size, T()); }
  iterator erase(iterator pos);
  iterator erase(iterator first, iterator last);
  void clear();

  /* 比较操作符的重载 */
  bool operator==(const deque<T, Allocator, BufSize> &rhs) {
	return size() == rhs.size() && std::equal(begin(), end(), rhs.begin());
  }
  bool operator!=(const deque<T, Allocator, BufSize> &rhs) { return !(*this == rhs); }
  bool operator<(const deque<T, Allocator, BufSize> &rhs) {
	return std::lexicographical_compare(begin(), end(), rhs.begin(),
										rhs.end());
  }
}; // deque end

template<typename T, typename Allocator, size_t BufSize>
void deque<T, Allocator, BufSize>::create_map_nodes(size_type num_element) {
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

template<typename T, typename Allocator, size_t BufSize>
void deque<T, Allocator, BufSize>::destroy_map_nodes() {
  for (map_pointer cur = start.node; cur <= finish.node; ++cur)
	deallocate_node(*cur);
  map_allocator::deallocate(map, map_size);
}

template<typename T, typename Allocator, size_t BufSize>
template<typename InputIterator>
void deque<T, Allocator, BufSize>::copy_init(InputIterator first, InputIterator last) {
  create_map_nodes(0);
  for (; first != last; ++first)
	push_back(*first);
}

template<typename T, typename Allocator, size_t BufSize>
void deque<T, Allocator, BufSize>::fill_init(size_type n, const value_type &value) {
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

template<typename T, typename Allocator, size_t BufSize>
void deque<T, Allocator, BufSize>::insert_aux(iterator pos, size_type n, const value_type &value) {
  const difference_type elems_before = pos - start;
  size_type length = size();
  if (elems_before < length / 2) {
	n = difference_type(n);
	iterator new_start = reserve_elements_at_front(n);
	iterator old_start = start;
	pos = start + elems_before;
	try {
	  if (elems_before >= n) {
		iterator start_n = start + n;
		uninitialized_copy(start, start_n, new_start);
		start = new_start;
		std::copy(start_n, pos, old_start);
		std::fill(pos - n, pos, value);
	  } else {
		iterator mid = uninitialized_copy(start, pos, new_start);
		uninitialized_fill(mid, start, value);
		start = new_start;
		std::fill(old_start, pos, value);
	  }
	} catch (...) {
	  destroy_nodes_at_front(new_start);
	}
  } else {
	iterator new_finish = reserve_elements_at_back(n);
	iterator old_finish = finish;
	n = difference_type(n);
	const difference_type elems_after =
		difference_type(length) - elems_before;
	pos = finish - elems_after;
	try {
	  if (elems_after > n) {
		iterator finish_n = finish - n;
		uninitialized_copy(finish_n, finish, finish);
		finish = new_finish;
		std::copy_backward(pos, finish_n, old_finish);
		std::fill(pos, pos + n, value);
	  } else {
		uninitialized_fill(finish, pos + n, value);
		uninitialized_copy(pos, finish, pos + n);
		finish = new_finish;
		fill(pos, old_finish, value);
	  }
	} catch (...) {
	  destroy_nodes_at_back(new_finish);
	}
  }
}

template<typename T, typename Allocator, size_t BufSize>
typename deque<T, Allocator, BufSize>::iterator deque<T, Allocator, BufSize>::reserve_elements_at_front(size_type n) {
  size_type remain = start.cur - start.first;
  if (n > remain) {
	size_type new_elements = n - remain;
	size_type new_nodes = (new_elements - 1) / buffer_size() + 1;
	reserve_map_at_front(new_nodes);
	size_type i;
	try {
	  for (i = 1; i <= new_nodes; ++i)
		*(start.node - i) = allocate_node();
	} catch (...) {
	  for (size_type j = 1; j < i; ++j)
		deallocate_node(*(start.node - j));
	  throw;
	}
  }
  return start - difference_type(n);
}

template<typename T, typename Allocator, size_t BufSize>
typename deque<T, Allocator, BufSize>::iterator deque<T, Allocator, BufSize>::reserve_elements_at_back(size_type n) {
  size_type remain = finish.last - finish.cur;
  if (n > remain) {
	size_type new_elements = n - remain;
	size_type new_nodes = (new_elements - 1) / buffer_size() + 1;
	reserve_map_at_back(new_nodes);
	size_type i;
	try {
	  for (i = 1; i <= new_nodes; ++i)
		*(finish.node + i) = allocate_node();
	} catch (...) {
	  for (size_type j = 1; j < i; ++j)
		deallocate_node(*(finish.node + j));
	  throw;
	}
  }
  return finish + difference_type(n);
}

template<typename T, typename Allocator, size_t BufSize>
void deque<T, Allocator, BufSize>::reallocate_map(size_type nodes_to_add, bool add_at_front) {
  size_type old_nodes_num = finish.node - start.node + 1;
  size_type new_nodes_num = old_nodes_num + nodes_to_add;
  map_pointer new_nstart;
  if (map_size > 2 * new_nodes_num) {
	new_nstart = map + (map_size - new_nodes_num) +
		(add_at_front ? nodes_to_add : 0);
	if (new_nstart < start.node)
	  std::copy(start.node, finish.node + 1, new_nstart);
	else
	  std::copy_backward(start.node, finish.node + 1,
						 new_nstart + new_nodes_num);
  } else {
	size_type new_map_size =
		map_size + std::max(map_size, nodes_to_add) + 2;
	map_pointer new_map = map_allocator::allocate(new_map_size);
	new_nstart = new_map + (new_map_size - new_nodes_num) +
		(add_at_front ? nodes_to_add : 0);
	std::copy(start.node, finish.node + 1, new_nstart);
	map_allocator::deallocate(map, map_size);
	map = new_map;
	map_size = new_map_size;
  }
  start.set_node(new_nstart);
  finish.set_node(new_nstart + old_nodes_num - 1);
}

template<typename T, typename Allocator, size_t BufSize>
deque<T, Allocator, BufSize> &deque<T, Allocator, BufSize>::operator=(const deque<T, Allocator, BufSize> &rhs) {
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

template<typename T, typename Allocator, size_t BufSize>
void deque<T, Allocator, BufSize>::swap(deque &rhs) {
  std::swap(start, rhs.start);
  std::swap(finish, rhs.finish);
  std::swap(map, rhs.map);
  std::swap(map_size, rhs.map_size);
}

template<typename T, typename Allocator, size_t BufSize>
void deque<T, Allocator, BufSize>::push_back(const value_type &value) {
  if (finish.cur != finish.last - 1) {
	construct(finish.cur, value);
	++finish.cur;
  } else {
	reserve_map_at_back();
	*(finish.node + 1) = allocate_node();
	try {
	  construct(finish.cur, value);
	  finish.set_node(finish.node + 1);
	  finish.cur = finish.first;
	} catch (...) {
	  deallocate_node(*(finish.node + 1));
	}
  }
}

template<typename T, typename Allocator, size_t BufSize>
void deque<T, Allocator, BufSize>::push_front(const value_type &value) {
  if (start.cur != start.first) {
	--start.cur;
	construct(start.cur, value);
  } else {
	reserve_map_at_front();
	*(start.node - 1) = allocate_node();
	try {
	  start.set_node(start.node - 1);
	  start.cur = start.last - 1;
	  construct(start.cur, value);
	} catch (...) {
	  deallocate_node(*(start.node - 1));
	}
  }
}

template<typename T, typename Allocator, size_t BufSize>
void deque<T, Allocator, BufSize>::pop_back() {
  if (finish.cur != finish.first) {
	--finish.cur;
	destroy(finish.cur);
  } else {
	deallocate_node(finish.first);
	finish.set_node(finish.node - 1);
	finish.cur = finish.last - 1;
	destroy(finish.cur);
  }
}

template<typename T, typename Allocator, size_t BufSize>
void deque<T, Allocator, BufSize>::pop_front() {
  destroy(start.cur);
  if (start.cur != start.last - 1) {
	++start.cur;
  } else {
	deallocate_node(start.first);
	start.set_node(start.node + 1);
	start.cur = start.first;
  }
}

template<typename T, typename Allocator, size_t BufSize>
typename deque<T, Allocator, BufSize>::iterator deque<T, Allocator, BufSize>::insert(iterator pos,
																					 const value_type &value) {
  if (pos.cur == start.cur) {
	push_front(value);
	return start;
  } else if (pos.cur == finish.cur) {
	push_back(value);
	iterator tmp = finish;
	--tmp;
	return tmp;
  } else {
	difference_type index = pos - start;
	if (index < size() < 2) {
	  push_front(front());
	  iterator front1 = start + 1;
	  iterator front2 = front1 + 1;
	  pos = start + index;
	  iterator pos1 = pos + 1;
	  std::copy(front2, pos1, front1);
	} else {
	  push_back(back());
	  iterator back1 = finish - 1;
	  iterator back2 = back1 - 1;
	  pos = start + index;
	  std::copy_backward(pos, back2, back1);
	}
	*pos = value;
	return pos;
  }
}

template<typename T, typename Allocator, size_t BufSize>
void deque<T, Allocator, BufSize>::insert(iterator pos, size_type n, const value_type &value) {
  if (pos.cur == start.cur) {
	iterator new_start = reserve_elements_at_front(n);
	uninitialized_fill(new_start, start, value);
	start = new_start;
  } else if (pos.cur == finish.cur) {
	iterator new_finish = reserve_elements_at_back(n);
	uninitialized_fill(finish, new_finish, value);
	finish = new_finish;
  } else
	insert_aux(pos, n, value);
}

template<typename T, typename Allocator, size_t BufSize>
template<typename InputIterator>
void deque<T, Allocator, BufSize>::insert(iterator pos, InputIterator first, InputIterator last) {
  copy(first, last, std::inserter(*this, pos));
}

template<typename T, typename Allocator, size_t BufSize>
void deque<T, Allocator, BufSize>::resize(size_type new_size, const value_type &value) {
  const size_type len = size();
  if (new_size < len)
	erase(start + new_size, finish);
  else
	insert(finish, new_size - len, value);
}

template<typename T, typename Allocator, size_t BufSize>
typename deque<T, Allocator, BufSize>::iterator deque<T, Allocator, BufSize>::erase(iterator pos) {
  iterator next = pos;
  ++next;
  difference_type index = pos - start;
  if (index < (size() / 2)) {
	std::copy_backward(start, pos, next);
	pop_front();
  } else {
	copy(next, finish, pos);
	pop_front();
  }
  return start + index;
}

template<typename T, typename Allocator, size_t BufSize>
typename deque<T, Allocator, BufSize>::iterator deque<T, Allocator, BufSize>::erase(iterator first, iterator last) {
  if (first == start && last == finish) {
	clear();
	return finish;
  } else {
	difference_type n = last - first;
	difference_type elems_before = first - start;
	if (elems_before < (size() - n) / 2) {
	  std::copy_backward(start, first, last);
	  iterator new_start = start + n;
	  destroy(start, new_start);
	  for (map_pointer cur = start.node; cur < new_start; ++cur)
		data_allocator::deallocate(*cur, buffer_size());
	  start = new_start;
	} else {
	  std::copy(last, finish, first);
	  iterator new_finish = finish - n;
	  destroy(new_finish, finish);
	  for (map_pointer cur = new_finish.node + 1; cur <= finish.node;
		   ++cur)
		data_allocator::deallocate(*cur, buffer_size());
	  finish = new_finish;
	}
	return start + elems_before;
  }
}

template<typename T, typename Allocator, size_t BufSize>
void deque<T, Allocator, BufSize>::clear() {
  for (map_pointer node = start.node + 1; node < finish.node; ++node) {
	destroy(*node, *node + buffer_size());
	data_allocator::deallocate(*node, buffer_size());
  }
  if (start.node != finish.node) {
	destroy(start.cur, start.last);
	destroy(finish.first, finish.cur);
	data_allocator::deallocate(finish.first, buffer_size());
  } else
	destroy(start.cur, finish.cur);
  finish = start;
}

}

#endif //TINYSTL__DEQUE_H_
