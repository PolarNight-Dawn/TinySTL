//
// Created by polarnight on 24-8-7, 下午5:39.
//

#ifndef TINYSTL__TREE_H_
#define TINYSTL__TREE_H_

/* <tree.h> 实现了 RB-tree（红黑树） */

#include "memory.h"

namespace tinystl {
using rb_tree_color_type = bool;
const rb_tree_color_type rb_tree_red = false;
const rb_tree_color_type rb_tree_black = true;

struct rb_tree_node_base {
  using color_type = rb_tree_color_type;
  using base_ptr = rb_tree_node_base *;

  color_type color;
  base_ptr parent;
  base_ptr left;
  base_ptr right;

  static base_ptr mininum(base_ptr x) {
	while (x->left != nullptr) x = x->left;
	return x;
  }

  static base_ptr maxinum(base_ptr x) {
	while (x->right != nullptr) x = x->right;
	return x;
  }
}; // tree node base end

template<typename Value>
struct rb_tree_node : public rb_tree_node_base {
  using link_type = rb_tree_node<Value> *;
  Value value_filed;
}; // tree node end

struct rb_tree_iterator_base {
  using base_ptr = rb_tree_node_base::base_ptr;
  using iterator_category = bidirectional_iterator_tag;
  using difference_type = ptrdiff_t;

  base_ptr node;

  rb_tree_iterator_base() : node(nullptr) {}

  void increment() {
	if (node->right != nullptr) {
	  node = node->right;
	  while (node->left != nullptr)
		node = node->left;
	} else {
	  base_ptr y = node->parent;
	  while (node == y->right) {
		node = y;
		y = y->parent;
	  }
	  if (node->right != y)
		node = y;
	}
  }

  void decrement() {
	if (node->color == rb_tree_red && node->parent->parent == node) {
	  node = node->right;
	} else if (node->left != nullptr) {
	  base_ptr y = node->left;
	  while (y->right != nullptr)
		y = y->right;
	  node = y;
	} else {
	  base_ptr y = node->parent;
	  while (node == y->left) {
		node = y;
		y = y->parent;
	  }
	  node = y;
	}
  }
};

template<typename Value, typename Ref, typename Ptr>
struct rb_tree_iterator : public rb_tree_iterator_base {
  using value_type = Value;
  using reference = Ref;
  using pointer = Ptr;
  using iterator = rb_tree_iterator<Value, Value &, Value *>;
  using const_iterator = rb_tree_iterator<Value, const Value &, const Value *>;
  using self = rb_tree_iterator<Value, Ref, Ptr>;
  using link_type = rb_tree_node<Value> *;

  rb_tree_iterator() = default;
  explicit rb_tree_iterator(link_type x) { node = x; }
  explicit rb_tree_iterator(const iterator &rhs) { node = rhs.node; }
  explicit rb_tree_iterator(const const_iterator &rhs) { node = rhs.node; }

  reference operator*() const { return static_cast<link_type>(node)->value_filed; }
  pointer operator->() const { return &(operator*()); }

  self &operator++() {
	increment();
	return *this;
  }
  self operator++(int) {
	self tmp = *this;
	increment();
	return tmp;
  }
  self &operator--() {
	decrement();
	return *this;
  }
  self operator--(int) {
	self tmp = *this;
	decrement();
	return tmp;
  }

  friend bool operator==(const self &x, const self &y) { return x.node == y.node; }
  friend bool operator!=(const self &x, const self &y) { return x.node != y.node; }
};

/* 左旋操作 */
template<typename Value>
inline void rb_tree_rotate_left(rb_tree_node_base *x, rb_tree_node_base *&root) {}

/* 右旋操作 */
template<typename Value>
inline void rb_tree_rotate_right(rb_tree_node_base *x, rb_tree_node_base *&root) {}

/* 调整 RB-tree */
template<typename Value>
inline void rb_tree_rebalance(rb_tree_node_base *x, rb_tree_node_base *&root) {}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator = Alloc>
class rb_tree {
 protected:
  using void_pointer = void *;
  using base_ptr = rb_tree_node_base *;
  using rb_tree_node = rb_tree_node<Value>;
  using rb_tree_node_allocator = alloc<rb_tree_node, Alloc>;
  using color_type = rb_tree_color_type;

 public:
  using key_type = Key;
  using value_type = Value;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using reference = value_type &;
  using const_reference = value_type &;
  using link_type = rb_tree_node *;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

  using iterator = rb_tree_iterator<value_type, reference, pointer>;
  using const_iterator = rb_tree_iterator<value_type, const_reference, const_pointer>;

 protected:
  size_type node_count;
  link_type header;
  Compare key_compare;

  /* 辅助函数 */
  link_type &root() const { return static_cast<link_type &>(header->parent); }
  link_type leftmost() const { return static_cast<link_type &>(header->left); }
  link_type rightmost() const { return static_cast<link_type &>(header->right); }

  link_type get_node() { return rb_tree_node_allocator::allocate(); }
  void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

  link_type create_node(const value_type &x) {
	link_type tmp = get_node();
	try {
	  construct(&tmp->value_filed, x);
	} catch (...) {
	  put_node(tmp);
	  throw;
	}
	return tmp;
  }

  link_type clone_node(link_type x) {
	link_type tmp = create_node(x->value_filed);
	tmp->color = x->color;
	tmp->left = nullptr;
	tmp->right = nullptr;
	return tmp;
  }

  void destroy_node(link_type p) {
	destory(&p->value_filed);
	put_node(p);
  }

  static link_type &left(link_type x) { return static_cast<link_type &>(x->left); }
  static link_type &right(link_type x) { return static_cast<link_type &>(x->right); }
  static link_type &parent(link_type x) { return static_cast<link_type &>(x->parent); }
  static reference value(link_type x) { return x->value_filed; }
  static const Key &key(link_type x) { return KeyOfValue()(value(x)); }
  static color_type &color(link_type x) { return static_cast<color_type &>(x->color); }

  static link_type mininum(link_type x) { return static_cast<link_type>(rb_tree_node_base::mininum(x)); }
  static link_type maxinum(link_type x) { return static_cast<link_type>(rb_tree_node_base::maxinum(x)); }

 private:
  iterator insert(base_ptr x, base_ptr y, const value_type *v);
  link_type copy(link_type x, link_type p);
  void erase(link_type x);
  void init();

 public:
  rb_tree(const Compare &comp = Compare()) : node_count(0), key_compare(comp) { init(); }

  ~rb_tree() {
	clear();
	put_node(header);
  }

  rb_tree<Key, Value, KeyOfValue, Compare, Allocator> &operator=(const rb_tree<Key,
																			   Value,
																			   KeyOfValue,
																			   Compare,
																			   Allocator> &x);

  rb_tree<Key, Value, KeyOfValue, Compare, Allocator> &operator=(rb_tree<Key,
																		 Value,
																		 KeyOfValue,
																		 Compare,
																		 Allocator> &x);

 public:
  Compare key_comp() const { return key_compare; }

  /* iterator 相关操作 */
  iterator begin() noexcept { return leftmost(); }
  const_iterator begin() const noexcept { return leftmost(); }
  iterator end() noexcept { return header; }
  const_iterator end() const noexcept { return header; }

  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }

  /* capacity 相关操作 */
  bool empty() const noexcept { return node_count == 0; }
  size_type size() const noexcept { return node_count; }
  size_type max_size() const noexcept { return static_cast<size_type>(-1); }

  /* container 相关操作 */
  std::pair<iterator, bool> insert_unique(const value_type &value);
  iterator insert_equal(const value_type &value);
  iterator insert_unique(iterator position, const value_type &value);
  iterator insert_equal(iterator position, const value_type &value);

  template<typename InputIterator>
  void insert_unique(InputIterator first, InputIterator last);
  template<typename InputIterator>
  void insert_equal(InputIterator first, InputIterator last);

  void erase(iterator position);
  size_type erase(const key_type &x);
  void erase(iterator first, iterator last);
  void erase(const key_type *first, const key_type *last);
  void clear();

  iterator find(const key_type &x);
  const_iterator find(const key_type &x) const;
  size_type count(const key_type &x) const;
  iterator lower_bound(const key_type &x);
  const_iterator lower_bound(const key_type &x) const;
  iterator upper_bound(const key_type &x);
  const_iterator upper_bound(const key_type &x) const;
  std::pair<iterator, iterator> equal_range(const key_type &x);
  std::pair<const_iterator, const_iterator> equal_range(const key_type &x) const;
  bool rb_verify() const;
}; // class rb_tree end

}

#endif //TINYSTL__TREE_H_
