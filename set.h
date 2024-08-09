//
// Created by polarnight on 24-8-9, 下午3:24.
//

#ifndef TINYSTL__SET_H_
#define TINYSTL__SET_H_

/* <set.h> 实现了 set/multiset
 * set：集合，键值即是实值，集合内元素自动排序，不允许键值重复
 * multiset：除了允许键值重复外，与 set 一致 */

#include <functional>

#include "tree.h"

namespace tinystl {
/* 缺省情况下采用递增排序 */
template<typename Key, typename Compare = std::less<Key>, typename Allocator = Alloc>
class set {
 public:
  using key_type = Key;
  using value_type = Key;
  /* 由于 set 键值即是实值，两者采用同一比较函数 */
  using key_compare = Compare;
  using value_compare = Compare;

 private:
  using base_ptr = rb_tree<key_type, value_type, std::_Identity<value_type>, key_compare, Allocator>;
  base_ptr rb_tree;

 public:
  using pointer = typename base_ptr::const_pointer;
  using const_point = typename base_ptr::const_pointer;
  using reference = typename base_ptr::const_reference;
  using const_reference = typename base_ptr::const_reference;
  // set iterator 无法执行写入操作，因为 set 元素有一定次序安排，不允许用户在任意位置进行写入操作
  using iterator = typename base_ptr::const_iterator;
  using const_iterator = typename base_ptr::const_iterator;
  using difference_type = typename base_ptr::difference_type;
  using size_type = typename base_ptr::size_type;

  set() : rb_tree(Compare()) {}
  explicit set(const Compare &comp) : rb_tree(comp) {}
  template<typename InputIterator>
  set(InputIterator first, InputIterator last) : rb_tree(Compare()) { rb_tree.insert_unique(first, last); }
  template<typename InputIterator>
  set(InputIterator first, InputIterator last, const Compare &comp) : rb_tree(comp) {
	rb_tree.insert_unique(first,
						  last);
  }
  explicit set(const set<Key, Compare, Alloc> &x) : rb_tree(x.rb_tree) {}
  set &operator=(const set &x) {
	rb_tree = x.rb_tree;
	return *this;
  }

  /* 以下为 set 所有操作，由于 <tree.h> 中的 RB-tree 实现均已提供，此处只做调用 */

  /* accessor 相关操作 */
  key_compare key_comp() const { return rb_tree.key_comp(); }
  value_compare value_comp() const { return key_comp(); }
  iterator begin() const { return rb_tree.begin(); }
  iterator end() const { return rb_tree.end(); }
  bool empty() const { return rb_tree.empty(); }
  size_type size() const { return rb_tree.size(); }
  size_type max_size() const { return rb_tree.max_size(); }
  void swap(set<Key, Compare, Alloc> &x) { rb_tree.swap(x.rb_tree); }

  /* container 相关操作 */
  using pair_iterator_boll = std::pair<iterator, bool>;

  pair_iterator_boll insert(const value_type &x) {
	std::pair<typename base_ptr::iterator, bool> p = rb_tree.insert_unique(x);
	return pair_iterator_boll(p.first, p.second);
  }
  iterator insert(iterator position, const value_type &x) {
	using base_iterator = typename base_ptr::iterator;
	return rb_tree.insert_unique(static_cast<base_iterator &>(position), x);
  }
  template<typename InputIterator>
  void insert(InputIterator first, InputIterator last) { rb_tree.insert_unique(first, last); }
  void erase(iterator position) {
	using base_iterator = typename base_ptr::iterator;
	rb_tree.erase(static_cast<base_iterator &>(position));
  }
  size_type erase(const key_type &x) { return rb_tree.erase(x); }
  void erase(iterator first, iterator last) {
	using base_iterator = typename base_ptr::iterator;
	rb_tree.erase(static_cast<base_iterator &>(first), static_cast<base_iterator &>(last));
  }
  void clear() { rb_tree.clear(); }

  /* operator 相关操作 */
  iterator find(const key_type &x) const { return rb_tree.find(x); }
  size_type count(const key_type &x) const { return rb_tree.count(x); }
  iterator lower_bound(const key_type &x) const { return rb_tree.lower_bound(x); }
  iterator upper_bound(const key_type &x) const { return rb_tree.upper_bound(x); }
  std::pair<iterator, iterator> equal_range(const key_type &x) const { return rb_tree.equal_range(x); }
  friend bool operator==(const set &lhs, const set &rhs) { return lhs.rb_tree == rhs.rb_tree; }
  friend bool operator<(const set &lhs, const set &rhs) { return lhs.rb_tree < rhs.rb_tree; }
}; // class set end

/* 重载函数 */
template<typename Key, typename Compare, typename Allocator>
bool operator==(const set<Key, Compare, Allocator> &lhs, const set<Key, Compare, Allocator> &rhs) {
  return lhs == rhs;
}

template<typename Key, typename Compare, typename Allocator>
bool operator<(const set<Key, Compare, Allocator> &lhs, const set<Key, Compare, Allocator> &rhs) {
  return lhs < rhs;
}

template<typename Key, typename Compare, typename Allocator>
bool operator!=(const set<Key, Compare, Allocator> &lhs, const set<Key, Compare, Allocator> &rhs) {
  return !(lhs == rhs);
}

template<typename Key, typename Compare, typename Allocator>
bool operator>(const set<Key, Compare, Allocator> &lhs, const set<Key, Compare, Allocator> &rhs) {
  return rhs < lhs;
}

template<typename Key, typename Compare, typename Allocator>
bool operator<=(const set<Key, Compare, Allocator> &lhs, const set<Key, Compare, Allocator> &rhs) {
  return !(rhs < lhs);
}

template<typename Key, typename Compare, typename Allocator>
bool operator>=(const set<Key, Compare, Allocator> &lhs, const set<Key, Compare, Allocator> &rhs) {
  return !(lhs < rhs);
}

template<typename Key, typename Compare, typename Allocator>
void swap(const set<Key, Compare, Allocator> &lhs, const set<Key, Compare, Allocator> &rhs) noexcept {
  lhs.swap(rhs);
}

/********************************************************************************************************************/

template<typename Key, typename Compare = std::less<Key>, typename Allocator = Alloc>
class multiset {
 public:
  using key_type = Key;
  using value_type = Key;
  /* 由于 multiset 键值即是实值，两者采用同一比较函数 */
  using key_compare = Compare;
  using value_compare = Compare;

 private:
  using base_ptr = rb_tree<key_type, value_type, std::_Identity<value_type>, key_compare, Allocator>;
  base_ptr rb_tree;

 public:
  using pointer = typename base_ptr::const_pointer;
  using const_point = typename base_ptr::const_pointer;
  using reference = typename base_ptr::const_reference;
  using const_reference = typename base_ptr::const_reference;
  // multiset iterator 无法执行写入操作，因为 set 元素有一定次序安排，不允许用户在任意位置进行写入操作
  using iterator = typename base_ptr::const_iterator;
  using const_iterator = typename base_ptr::const_iterator;
  using difference_type = typename base_ptr::difference_type;
  using size_type = typename base_ptr::size_type;

  multiset() : rb_tree(Compare()) {}
  explicit multiset(const Compare &comp) : rb_tree(comp) {}
  template<typename InputIterator>
  multiset(InputIterator first, InputIterator last) : rb_tree(Compare()) { rb_tree.insert_equal(first, last); }
  template<typename InputIterator>
  multiset(InputIterator first, InputIterator last, const Compare &comp) : rb_tree(comp) {
	rb_tree.insert_equal(first,
						 last);
  }
  explicit multiset(const set<Key, Compare, Alloc> &x) : rb_tree(x.rb_tree) {}
  multiset &operator=(const multiset &x) {
	rb_tree = x.rb_tree;
	return *this;
  }

  /* 以下为 multiset 所有操作，由于 <tree.h> 中的 RB-tree 实现均已提供，此处只做调用 */

  /* accessor 相关操作 */
  key_compare key_comp() const { return rb_tree.key_comp(); }
  value_compare value_comp() const { return key_comp(); }
  iterator begin() const { return rb_tree.begin(); }
  iterator end() const { return rb_tree.end(); }
  bool empty() const { return rb_tree.empty(); }
  size_type size() const { return rb_tree.size(); }
  size_type max_size() const { return rb_tree.max_size(); }
  void swap(set<Key, Compare, Alloc> &x) { rb_tree.swap(x.rb_tree); }

  /* container 相关操作 */
  using pair_iterator_boll = std::pair<iterator, bool>;

  pair_iterator_boll insert(const value_type &x) {
	std::pair<typename base_ptr::iterator, bool> p = rb_tree.insert_equal(x);
	return pair_iterator_boll(p.first, p.second);
  }
  iterator insert(iterator position, const value_type &x) {
	using base_iterator = typename base_ptr::iterator;
	return rb_tree.insert_equal(static_cast<base_iterator &>(position), x);
  }
  template<typename InputIterator>
  void insert(InputIterator first, InputIterator last) { rb_tree.insert_equal(first, last); }
  void erase(iterator position) {
	using base_iterator = typename base_ptr::iterator;
	rb_tree.erase(static_cast<base_iterator &>(position));
  }
  size_type erase(const key_type &x) { return rb_tree.erase(x); }
  void erase(iterator first, iterator last) {
	using base_iterator = typename base_ptr::iterator;
	rb_tree.erase(static_cast<base_iterator &>(first), static_cast<base_iterator &>(last));
  }
  void clear() { rb_tree.clear(); }

  /* operator 相关操作 */
  iterator find(const key_type &x) const { return rb_tree.find(x); }
  size_type count(const key_type &x) const { return rb_tree.count(x); }
  iterator lower_bound(const key_type &x) const { return rb_tree.lower_bound(x); }
  iterator upper_bound(const key_type &x) const { return rb_tree.upper_bound(x); }
  std::pair<iterator, iterator> equal_range(const key_type &x) const { return rb_tree.equal_range(x); }
  friend bool operator==(const multiset &lhs, const multiset &rhs) { return lhs.tree_ == rhs.tree_; }
  friend bool operator<(const multiset &lhs, const multiset &rhs) { return lhs.tree_ < rhs.tree_; }
}; // class multiset end

/* 重载函数 */
template<typename Key, typename Compare, typename Allocator>
bool operator==(const multiset<Key, Compare, Allocator> &lhs, const multiset<Key, Compare, Allocator> &rhs) {
  return lhs == rhs;
}

template<typename Key, typename Compare, typename Allocator>
bool operator<(const multiset<Key, Compare, Allocator> &lhs, const multiset<Key, Compare, Allocator> &rhs) {
  return lhs < rhs;
}

template<typename Key, typename Compare, typename Allocator>
bool operator!=(const multiset<Key, Compare, Allocator> &lhs, const multiset<Key, Compare, Allocator> &rhs) {
  return !(lhs == rhs);
}

template<typename Key, typename Compare, typename Allocator>
bool operator>(const multiset<Key, Compare, Allocator> &lhs, const multiset<Key, Compare, Allocator> &rhs) {
  return rhs < lhs;
}

template<typename Key, typename Compare, typename Allocator>
bool operator<=(const multiset<Key, Compare, Allocator> &lhs, const multiset<Key, Compare, Allocator> &rhs) {
  return !(rhs < lhs);
}

template<typename Key, typename Compare, typename Allocator>
bool operator>=(const multiset<Key, Compare, Allocator> &lhs, const multiset<Key, Compare, Allocator> &rhs) {
  return !(lhs < rhs);
}

template<typename Key, typename Compare, typename Allocator>
void swap(const multiset<Key, Compare, Allocator> &lhs, const multiset<Key, Compare, Allocator> &rhs) noexcept {
  lhs.swap(rhs);
}

} // namespace tinystl

#endif //TINYSTL__SET_H_
