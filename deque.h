//
// Created by polarnight on 24-8-5, 下午4:40.
//

#ifndef TINYSTL__DEQUE_H_
#define TINYSTL__DEQUE_H_

#include <initializer_list>

#include "memory.h"

namespace tinystl {
inline size_t deque_buf_size(size_t n, size_t sz) {
  return n != 0 ? n : (sz < 512 ? static_cast<size_t>(512 / sz) : static_cast<size_t>(1));
}

template<typename T, typename Ref, typename Ptr, size_t BufSize>
struct deque_iterator {
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

}

#endif //TINYSTL__DEQUE_H_
