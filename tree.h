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

struct _rb_tree_node_base {
  using color_type = rb_tree_color_type;
  using base_ptr = _rb_tree_node_base *;

  color_type color;
  base_ptr parent;
  base_ptr left;
  base_ptr right;

  static base_ptr minimum(base_ptr x) {
	while (x->left != nullptr) x = x->left;
	return x;
  }

  static base_ptr maximum(base_ptr x) {
	while (x->right != nullptr) x = x->right;
	return x;
  }
}; // tree node base end

template<typename Value>
struct _rb_tree_node : public _rb_tree_node_base {
  using link_type = _rb_tree_node<Value> *;
  Value value_filed;
}; // tree node end

struct rb_tree_iterator_base {
  using base_ptr = _rb_tree_node_base::base_ptr;
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
  using link_type = _rb_tree_node<Value> *;

  rb_tree_iterator() = default;
  rb_tree_iterator(link_type x) { node = x; }
  rb_tree_iterator(const iterator &rhs) { node = rhs.node; }
  rb_tree_iterator(const const_iterator &rhs) { node = rhs.node; }

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
inline void rb_tree_rotate_left(_rb_tree_node_base *x, _rb_tree_node_base *&root) {
  _rb_tree_node_base *y = x->right;
  x->right = y->left;
  if (y->left != nullptr) y->left->parent = x;
  y->parent = x->parent;

  if (x == root) root = y;
  else if (x == x->parent->left) x->parent->left = y;
  else x->parent->right = y;
  y->left = x;
  x->parent = y;
}

/* 右旋操作 */
inline void rb_tree_rotate_right(_rb_tree_node_base *x, _rb_tree_node_base *&root) {
  _rb_tree_node_base *y = x->left;
  x->left = y->right;
  if (y->right != nullptr) y->right->parent = x;
  y->parent = x->parent;

  if (x == root)root = y;
  else if (x == x->parent->right) x->parent->right = y;
  else x->parent->left = y;
  y->right = x;
  x->parent = y;
}

/* 调整 RB-tree */
inline void rb_tree_rebalance(_rb_tree_node_base *x, _rb_tree_node_base *&root) {
  x->color = rb_tree_red;
  while (x != root && x->parent->color == rb_tree_red) {
	if (x->parent == x->parent->parent->left) {
	  _rb_tree_node_base *y = x->parent->parent->right;
	  if (y && y->color == rb_tree_red) {
		x->parent->color = rb_tree_black;
		y->color = rb_tree_black;
		x->parent->parent->color = rb_tree_red;
		x = x->parent->parent;
	  } else {
		if (x == x->parent->right) {
		  x = x->parent;
		  rb_tree_rotate_left(x, root);
		}
		x->parent->color = rb_tree_black;
		x->parent->parent->color = rb_tree_red;
		rb_tree_rotate_right(x->parent->parent, root);
	  }
	} else {
	  _rb_tree_node_base *y = x->parent->parent->left;
	  if (y && y->color == rb_tree_red) {
		x->parent->color = rb_tree_black;
		y->color = rb_tree_black;
		x->parent->parent->color = rb_tree_red;
		x = x->parent->parent;
	  } else {
		if (x == x->parent->left) {
		  x = x->parent;
		  rb_tree_rotate_right(x, root);
		}
		x->parent->color = rb_tree_black;
		x->parent->parent->color = rb_tree_red;
		rb_tree_rotate_left(x->parent->parent, root);
	  }
	}
  }
  root->color = rb_tree_black;
}

inline _rb_tree_node_base *rb_tree_rebalance_for_erase(_rb_tree_node_base *z,
													   _rb_tree_node_base *&root,
													   _rb_tree_node_base *&leftmost,
													   _rb_tree_node_base *&rightmost) {
  _rb_tree_node_base *y = nullptr;
  _rb_tree_node_base *x = nullptr;
  _rb_tree_node_base *x_parent = nullptr;
  if (y->left == nullptr)        // z has at most one non-null child. y == z.
	x = y->right;        // x might be null.
  else if (y->right == nullptr)  // z has exactly one non-null child.  y == z.
	x = y->left;         // x is not null.
  else {                   // z has two non-null children.  Set y to
	y = y->right;        //   z's successor.  x might be null.
	while (y->left != nullptr)
	  y = y->left;
	x = y->right;
  }
  if (y != z) {  // relink y in place of z.  y is z's successor
	z->left->parent = y;
	y->left = z->left;
	if (y != z->right) {
	  x_parent = y->parent;
	  if (x) x->parent = y->parent;
	  y->parent->left = x;  // y must be a left child
	  y->right = z->right;
	  z->right->parent = y;
	} else
	  x_parent = y;
	if (root == z)
	  root = y;
	else if (z->parent->left == z)
	  z->parent->left = y;
	else
	  z->parent->right = y;
	y->parent = z->parent;
	std::swap(y->color, z->color);
	y = z;
  } else {  // y == z
	x_parent = y->parent;
	if (x)
	  x->parent = y->parent;
	if (root == z)
	  root = x;
	else if (z->parent->left == z)
	  z->parent->left = x;
	else
	  z->parent->right = x;
	if (leftmost == z)
	  if (z->right == nullptr)  // z->left must be null also
		leftmost = z->parent;
	  else
		leftmost = _rb_tree_node_base::minimum(x);
	if (rightmost == z)
	  if (z->left == nullptr)  // z->right must be null also
		rightmost = z->parent;
	  else  // x == z->left
		rightmost = _rb_tree_node_base::maximum(x);
  }
  if (y->color != rb_tree_red) {
	while (x != root && (x == nullptr || x->color == rb_tree_black))
	  if (x == x_parent->left) {
		_rb_tree_node_base *w = x_parent->right;
		if (w->color == rb_tree_red) {
		  w->color = rb_tree_black;
		  x_parent->color = rb_tree_red;
		  rb_tree_rotate_left(x_parent, root);
		  w = x_parent->right;
		}
		if ((w->left == nullptr || w->left->color == rb_tree_black) &&
			(w->right == nullptr || w->right->color == rb_tree_black)) {
		  w->color = rb_tree_red;
		  x = x_parent;
		  x_parent = x_parent->parent;
		} else {
		  if (w->right == nullptr || w->right->color == rb_tree_black) {
			if (w->left)
			  w->left->color = rb_tree_black;
			w->color = rb_tree_red;
			rb_tree_rotate_right(w, root);
			w = x_parent->right;
		  }
		  w->color = x_parent->color;
		  x_parent->color = rb_tree_black;
		  if (w->right)
			w->right->color = rb_tree_black;
		  rb_tree_rotate_left(x_parent, root);
		  break;
		}
	  } else {  // same as above, with right <-> left.
		_rb_tree_node_base *w = x_parent->left;
		if (w->color == rb_tree_red) {
		  w->color = rb_tree_black;
		  x_parent->color = rb_tree_red;
		  rb_tree_rotate_right(x_parent, root);
		  w = x_parent->left;
		}
		if ((w->right == nullptr || w->right->color == rb_tree_black) &&
			(w->left == nullptr || w->left->color == rb_tree_black)) {
		  w->color = rb_tree_red;
		  x = x_parent;
		  x_parent = x_parent->parent;
		} else {
		  if (w->left == nullptr || w->left->color == rb_tree_black) {
			if (w->right)
			  w->right->color = rb_tree_black;
			w->color = rb_tree_red;
			rb_tree_rotate_left(w, root);
			w = x_parent->left;
		  }
		  w->color = x_parent->color;
		  x_parent->color = rb_tree_black;
		  if (w->left)
			w->left->color = rb_tree_black;
		  rb_tree_rotate_right(x_parent, root);
		  break;
		}
	  }
	if (x)
	  x->color = rb_tree_black;
  }
  return y;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare = std::less<Key>, typename Allocator = Alloc>
class rb_tree {
 protected:
  using void_pointer = void *;
  using base_ptr = _rb_tree_node_base *;
  using rb_tree_node = _rb_tree_node<Value>;
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
  using reverse_iterator = tinystl::reverse_iterator<iterator>;
  using const_reverse_iterator = tinystl::reverse_iterator<const_iterator>;

 protected:
  size_type node_count{};
  link_type header;
  Compare key_compare;

  /* 辅助函数 */
  link_type &root() const { return (link_type &)(header->parent); }
  link_type &leftmost() const { return (link_type &)(header->left); }
  link_type &rightmost() const { return (link_type &)(header->right); }

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
	destroy(&p->value_filed);
	put_node(p);
  }

  static link_type &left(link_type x) { return (link_type &)(x->left); }
  static link_type &right(link_type x) { return (link_type &)(x->right); }
  static link_type &parent(link_type x) { return (link_type &)(x->parent); }
  static reference value(link_type x) { return x->value_filed; }
  static const Key &key(link_type x) { return KeyOfValue()(value(x)); }
  static color_type &color(link_type x) { return static_cast<color_type &>(x->color); }

  static link_type minimum(link_type x) { return (link_type)(_rb_tree_node_base::minimum(x)); }
  static link_type maximum(link_type x) { return (link_type)(_rb_tree_node_base::maximum(x)); }

 private:
  iterator insert_aux(base_ptr x, base_ptr y, const value_type &v);
  link_type copy_aux(link_type x, link_type p) {
	link_type top = clone_node(x);
	top->parent = p;
	try {
	  if (x->right)
		top->right = copy_aux(right(x), top);
	  p = top;
	  x = left(x);

	  while (x != 0) {
		link_type y = clone_node(x);
		p->left = y;
		y->parent = p;
		if (x->right)
		  y->right = copy_aux(right(x), y);
		p = y;
		x = left(x);
	  }
	} catch (...) {
	  (erase_aux(top));
	  throw;
	}
	return top;
  }
  void erase_aux(link_type x);
  void init() {
	header = get_node();
	color(header) = rb_tree_red;
	root() = 0;
	leftmost() = header;
	rightmost() = header;
  }
  void reset() {
	header = nullptr;
	node_count = 0;
  }

 public:
  rb_tree(const Compare &comp = Compare()) : node_count(0), key_compare(comp) { init(); }
  rb_tree(const rb_tree &rhs) {
	header = get_node();
	color(header) = rb_tree_red;
	if (rhs.root() == NULL) {
	  root() = NULL;
	  leftmost() = header;
	  rightmost() = header;
	} else {
	  try {
		root() = copy_aux(rhs.root(), header);
	  } catch (...) {
		put_node(header);
		throw;
	  }
	  leftmost() = minimum(root());
	  rightmost() = maximun(root());
	}
	node_count = rhs.node_count;
	key_compare = rhs.key_compare;
  }

  ~rb_tree() {
	clear();
	put_node(header);
  }

  rb_tree &operator=(const rb_tree &rhs);

  rb_tree &operator=(rb_tree &&rhs) noexcept;

 public:
  Compare key_comp() const { return key_compare; }

  /* iterator 相关操作 */
  iterator begin() noexcept { return iterator(leftmost()); }
  const_iterator begin() const noexcept { return const_iterator(leftmost()); }
  iterator end() noexcept { return iterator(header); }
  const_iterator end() const noexcept { return const_iterator(header); }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }

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
  void clear() {
	if (node_count != 0) {
	  erase_aux(root());
	  leftmost() = header;
	  root() = 0;
	  rightmost() = header;
	  node_count = 0;
	}
  }

  iterator find(const key_type &k);
  const_iterator find(const key_type &k) const;
  size_type count(const key_type &k) const;
  iterator lower_bound(const key_type &k);
  const_iterator lower_bound(const key_type &k) const;
  iterator upper_bound(const key_type &k);
  const_iterator upper_bound(const key_type &k) const;
  std::pair<iterator, iterator> equal_range(const key_type &k);
  std::pair<const_iterator, const_iterator> equal_range(const key_type &k) const;
  int black_count(base_ptr node, base_ptr root);
  bool rb_verify() const;
}; // class rb_tree end

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
inline bool operator==(const rb_tree<Key, Value, KeyOfValue, Compare, Allocator> &lhs,
					   const rb_tree<Key, Value, KeyOfValue, Compare, Allocator> &rhs) {
  return lhs.size() == rhs.size() && equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
inline bool operator<(const rb_tree<Key, Value, KeyOfValue, Compare, Allocator> &lhs,
					  const rb_tree<Key, Value, KeyOfValue, Compare, Allocator> &rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
inline void swap(rb_tree<Key, Value, KeyOfValue, Compare, Allocator> &lhs,
				 rb_tree<Key, Value, KeyOfValue, Compare, Allocator> &rhs) {
  lhs.swap(rhs);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
rb_tree<Key, Value, KeyOfValue, Compare, Allocator> &
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::operator=(const rb_tree &rhs) {
  if (this != &rhs) {
	clear();
	node_count = 0;
	key_compare = rhs.key_compare;
	if (rhs.root() == 0) {
	  root() = 0;
	  leftmost() = header;
	  rightmost() = header;
	} else {
	  root() = copy_aux(rhs.root(), header);
	  leftmost() = minimum(root());
	  rightmost() = maximum(root());
	  node_count = rhs.node_count;
	}
  }
  return *this;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
rb_tree<Key, Value, KeyOfValue, Compare, Allocator> &
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::
operator=(rb_tree &&rhs) noexcept {
  clear();
  header = std::move(rhs.header);
  node_count = rhs.node_count;
  key_compare = rhs.key_compare;
  rhs.reset();
  return *this;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::insert_aux(base_ptr x_,
																base_ptr y_,
																const Value &v) {
  link_type x = static_cast<link_type>(x_);
  link_type y = static_cast<link_type>(y_);
  link_type z;
  if (y == header || x != nullptr || key_compare(KeyOfValue()(v), key(y))) {
	z = create_node(v);
	left(y) = z;
	if (y == header) {
	  root() = z;
	  rightmost() = z;
	} else if (y == leftmost())
	  leftmost() = z;
  } else {
	z = create_node(v);
	right(y) = z;
	if (y == rightmost())
	  rightmost() = z;
  }
  parent(z) = y;
  left(z) = 0;
  right(z) = 0;
  rb_tree_rebalance(z, header->parent);
  ++node_count;
  return iterator(z);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::insert_equal(const Value &v) {
  link_type y = header;
  link_type x = root();
  while (x != 0) {
	y = x;
	x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
  }
  return insert_aux(x, y, v);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::iterator, bool>
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::insert_unique(const Value &v) {
  link_type y = header;
  link_type x = root();
  bool comp = true;
  while (x != 0) {
	y = x;
	comp = key_compare(KeyOfValue()(v), key(x));
	x = comp ? left(x) : right(x);
  }
  iterator j = iterator(y);
  if (comp)
	if (j == begin())
	  return std::pair<iterator, bool>(insert_aux(x, y, v), true);
	else
	  --j;
  if (key_compare(key(j.node), KeyOfValue()(v)))
	return std::pair<iterator, bool>(insert_aux(x, y, v), true);
  return std::pair<iterator, bool>(j, false);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::insert_unique(iterator position,
																   const Value &v) {
  link_type pos_node = static_cast<link_type>(position.node);
  if (pos_node == header->left)
	if (size() > 0 && key_compare(KeyOfValue()(v), key(pos_node)))
	  return insert_aux(pos_node, pos_node, v);
	else
	  return insert_unique(v).first;
  else if (pos_node == header)
	if (key_compare(key(rightmost()), KeyOfValue()(v)))
	  return insert_aux(0, rightmost(), v);
	else
	  return insert_unique(v).first;
  else {
	iterator before = position;
	link_type bef_node = static_cast<link_type>(before.node);
	--before;
	if (key_compare(key(bef_node), KeyOfValue()(v)) &&
		key_compare(KeyOfValue()(v), key(pos_node)))
	  if (right(bef_node) == 0)
		return insert_aux(0, before.node, v);
	  else
		return insert_aux(position.node, position.node, v);
	else
	  return insert_unique(v).first;
  }
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::insert_equal(iterator position,
																  const Value &v) {
  link_type pos_node = static_cast<link_type>(position.node);
  if (pos_node == header->left)
	if (size() > 0 && key_compare(KeyOfValue()(v), key((link_type)pos_node)))
	  return insert_aux(pos_node, pos_node, v);
	else
	  return insert_equal(v);
  else if (pos_node == header)
	if (!key_compare(KeyOfValue()(v), key(rightmost())))
	  return insert_aux(0, rightmost(), v);
	else
	  return insert_equal(v);
  else {
	iterator before = position;
	link_type bef_node = static_cast<link_type>(before.node);
	--before;
	if (!key_compare(KeyOfValue()(v), key(bef_node)) &&
		!key_compare(key(pos_node), KeyOfValue()(v)))
	  if (right(bef_node) == 0)
		return insert_aux(0, before.node, v);
	  else
		return insert_aux(position.node, position.node, v);
	  // first argument just needs to be non-null
	else
	  return insert_equal(v);
  }
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
template<typename II>
void rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::insert_equal(II first, II last) {
  for (; first != last; ++first)
	insert_equal(*first);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
template<typename II>
void rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::insert_unique(II first, II last) {
  for (; first != last; ++first)
	insert_unique(*first);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
inline void rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::erase(iterator position) {
  link_type y = (link_type)rb_tree_rebalance_for_erase(
	  position.node, header->parent, header->left, header->right);
  destroy_node(y);
  --node_count;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::size_type
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::erase(const Key &x) {
  std::pair<iterator, iterator> p = equal_range(x);
  size_type n = 0;
  distance(p.first, p.second, n);
  erase(p.first, p.second);
  return n;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
void rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::erase_aux(link_type
x) {
while (x != 0) {
erase_aux(right(x)
);
link_type y = left(x);
destroy_node(x);
x = y;
}
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
void rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::erase(iterator first,
																iterator last) {
  if (first == begin() && last == end())
	clear();
  else
	while (first != last)
	  erase(first++);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
void rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::erase(const Key *first,
																const Key *last) {
  while (first != last)
	erase(*first++);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::find(const Key &k) {
  link_type y = header;
  link_type x = root();

  while (x != 0)
	if (!key_compare(key(x), k))
	  y = x, x = left(x);
	else
	  x = right(x);

  iterator j = iterator(y);
  return (j == end() || key_compare(k, key(j.node))) ? end() : j;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::const_iterator
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::find(const Key &k) const {
  link_type y = header;
  link_type x = root();

  while (x != 0) {
	if (!key_compare(key(x), k))
	  y = x, x = left(x);
	else
	  x = right(x);
  }
  const_iterator j = const_iterator(y);
  return (j == end() || key_compare(k, key(j.node))) ? end() : j;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::size_type
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::count(const Key &k) const {
  std::pair<const_iterator, const_iterator> p = equal_range(k);
  size_type n = 0;
  distance(p.first, p.second, n);
  return n;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::lower_bound(const Key &k) {
  link_type y = header;
  link_type x = root();

  while (x != 0)
	if (!key_compare(key(x), k))
	  y = x, x = left(x);
	else
	  x = right(x);
  return iterator(y);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::const_iterator
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::lower_bound(const Key &k) const {
  link_type y = header;
  link_type x = root();

  while (x != 0)
	if (!key_compare(key(x), k))
	  y = x, x = left(x);
	else
	  x = right(x);
  return const_iterator(y);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::upper_bound(const Key &k) {
  link_type y = header;
  link_type x = root();

  while (x != 0)
	if (key_compare(k, key(x)))
	  y = x, x = left(x);
	else
	  x = right(x);
  return iterator(y);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::const_iterator
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::upper_bound(const Key &k) const {
  link_type y = header;
  link_type x = root();

  while (x != 0)
	if (key_compare(k, key(x)))
	  y = x, x = left(x);
	else
	  x = right(x);
  return const_iterator(y);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
inline std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::iterator,
				 typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::iterator>
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::equal_range(const Key &k) {
  return std::pair<iterator, iterator>(lower_bound(k), upper_bound(k));
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
inline std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::const_iterator,
				 typename rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::const_iterator>
rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::equal_range(const Key &k) const {
  return std::pair<const_iterator, const_iterator>(lower_bound(k),
												   upper_bound(k));
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
inline int rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::black_count(base_ptr node, base_ptr root) {
  if (node == nullptr)
	return 0;
  else {
	int bc = node->color == rb_tree_black ? 1 : 0;
	if (node == root)
	  return bc;
	else
	  return bc + black_count(node->parent, root);
  }
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
bool rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::rb_verify() const {
  if (node_count == 0 || begin() == end())
	return node_count == 0 && begin() == end() && header->left == header &&
		header->right == header;

  int len = black_count(leftmost(), root());
  for (const_iterator it = begin(); it != end(); ++it) {
	link_type
		x = (link_type)
	it.node;
	link_type L = left(x);
	link_type R = right(x);

	if (x->color == rb_tree_red)
	  if ((L && L->color == rb_tree_red) || (R && R->color == rb_tree_red))
		return false;

	if (L && key_compare(key(x), key(L)))
	  return false;
	if (R && key_compare(key(R), key(x)))
	  return false;

	if (!L && !R && black_count(x, root()) != len)
	  return false;
  }

  if (leftmost() != _rb_tree_node_base::minimum(root()))
	return false;
  if (rightmost() != _rb_tree_node_base::maximum(root()))
	return false;

  return true;
}

}

#endif //TINYSTL__TREE_H_
