//
// Created by polarnight on 24-8-9, 下午8:43.
//

#ifndef TINYSTL__MAP_H_
#define TINYSTL__MAP_H_

/* <map.h> 实现了 map/multimap
 * map：映射，元素具有键值和实值，会根据键值大小自动排序，不允许键值重复
 * multimap：除了允许键值重复外，与 map 一致 */

#include <functional>

#include "tree.h"

namespace tinystl {
/* 缺省情况下采用递增排序 */
template<typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = Alloc>
class map {
 public:
  using key_type = Key;
  using data_type = T;
  using mapped_type = T;
  using value_type = std::pair<const Key, T>;
  using key_compare = Compare;

  /* 定义一个 functor，用来进行元素比较 */
  class value_compare : public std::binary_function<value_type, value_type, bool> {
	friend class map<Key, T, Compare, Allocator>;
   private:
	Compare comp;
	explicit value_compare(Compare c) : comp(c) {}
   public:
	// 比较键值的大小
	bool operator()(const value_type &lhs, const value_type &rhs) const { return comp(lhs.first, rhs.first); }
  };

 private:
  using base_ptr = rb_tree<key_type, value_type, std::_Select1st<value_type>, key_compare, Allocator>;
  base_ptr rb_tree;

 public:
  using pointer = typename base_ptr::const_pointer;
  using const_point = typename base_ptr::const_pointer;
  using reference = typename base_ptr::const_reference;
  using const_reference = typename base_ptr::const_reference;
  // map 允许用通过其迭代器修改元素的实值 value_type
  using iterator = typename base_ptr::iterator;
  using const_iterator = typename base_ptr::const_iterator;
  using difference_type = typename base_ptr::difference_type;
  using size_type = typename base_ptr::size_type;

  map() : rb_tree(Compare()) {}
  explicit map(const Compare &comp) : rb_tree(comp) {}
  template<typename InputIterator>
  map(InputIterator first, InputIterator last) :rb_tree(Compare()) { rb_tree.insert_unique(first, last); }
  template<typename InputIterator>
  map(InputIterator first, InputIterator last, const Compare &comp) :rb_tree(comp) {
	rb_tree.insert_unique(first,
						  last);
  }
  explicit map(const map<Key, Compare, Alloc> &x) : rb_tree(x.rb_tree) {}
  map &operator=(const map &x) {
	rb_tree = x.rb_tree;
	return *this;
  }

  /* 以下为 map 所有操作，由于 <tree.h> 中的 RB-tree 实现均已提供，此处只做调用 */

  /* accessor 相关操作 */
  key_compare key_comp() const { return rb_tree.key_comp(); }
  value_compare value_comp() const { return key_comp(); }
  iterator begin() noexcept { return rb_tree.begin(); }
  const_iterator begin() const noexcept { return rb_tree.begin(); }
  iterator end() noexcept { return rb_tree.end(); }
  const_iterator end() const noexcept { return rb_tree.end(); }
  bool empty() const noexcept { return rb_tree.empty(); }
  size_type size() const noexcept { return rb_tree.size(); }
  size_type max_size() const noexcept { return rb_tree.max_size(); }
  void swap(map<Key, Compare, Alloc> &x) noexcept { rb_tree.swap(x.rb_tree); }

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
  iterator find(const key_type &x) { return rb_tree.find(x); }
  const_iterator find(const key_type &x) const { return rb_tree.find(x); }
  size_type count(const key_type &x) const { return rb_tree.count(x); }
  iterator lower_bound(const key_type &x) { return rb_tree.lower_bound(x); }
  const_iterator lower_bound(const key_type &x) const { return rb_tree.lower_bound(x); }
  iterator upper_bound(const key_type &x) { return rb_tree.upper_bound(x); }
  const_iterator upper_bound(const key_type &x) const { return rb_tree.upper_bound(x); }
  std::pair<iterator, iterator> equal_range(const key_type &x) { return rb_tree.equal_range(x); }
  std::pair<iterator, iterator> equal_range(const key_type &x) const { return rb_tree.equal_range(x); }
  friend bool operator==(const map &lhs, const map &rhs) { return lhs.rb_tree == rhs.rb_tree; }
  friend bool operator<(const map &lhs, const map &rhs) { return lhs.rb_tree < rhs.rb_tree; }
}; // class map end

/* 重载函数 */
template<typename Key, typename Compare, typename Allocator>
inline bool operator==(const map<Key, Compare, Allocator> &lhs, const map<Key, Compare, Allocator> &rhs) {
  return lhs == rhs;
}

template<typename Key, typename Compare, typename Allocator>
inline bool operator<(const map<Key, Compare, Allocator> &lhs, const map<Key, Compare, Allocator> &rhs) {
  return lhs < rhs;
}

template<typename Key, typename Compare, typename Allocator>
inline bool operator!=(const map<Key, Compare, Allocator> &lhs, const map<Key, Compare, Allocator> &rhs) {
  return !(lhs == rhs);
}

template<typename Key, typename Compare, typename Allocator>
inline bool operator>(const map<Key, Compare, Allocator> &lhs, const map<Key, Compare, Allocator> &rhs) {
  return rhs < lhs;
}

template<typename Key, typename Compare, typename Allocator>
inline bool operator<=(const map<Key, Compare, Allocator> &lhs, const map<Key, Compare, Allocator> &rhs) {
  return !(rhs < lhs);
}

template<typename Key, typename Compare, typename Allocator>
inline bool operator>=(const map<Key, Compare, Allocator> &lhs, const map<Key, Compare, Allocator> &rhs) {
  return !(lhs < rhs);
}

template<typename Key, typename Compare, typename Allocator>
inline void swap(const map<Key, Compare, Allocator> &lhs, const map<Key, Compare, Allocator> &rhs) noexcept {
  lhs.swap(rhs);
}

/********************************************************************************************************************/

template<typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = Alloc>
class multimap {
 public:
  using key_type = Key;
  using data_type = T;
  using mapped_type = T;
  using value_type = std::pair<const Key, T>;
  using key_compare = Compare;

  /* 定义一个 functor，用来进行元素比较 */
  class value_compare : public std::binary_function<value_type, value_type, bool> {
	friend class map<Key, T, Compare, Allocator>;
   private:
	Compare comp;
	explicit value_compare(Compare c) : comp(c) {}
   public:
	// 比较键值的大小
	bool operator()(const value_type &lhs, const value_type &rhs) const { return comp(lhs.first, rhs.first); }
  };

 private:
  using base_ptr = rb_tree<key_type, value_type, std::_Select1st<value_type>, key_compare, Allocator>;
  base_ptr rb_tree;

 public:
  using pointer = typename base_ptr::const_pointer;
  using const_point = typename base_ptr::const_pointer;
  using reference = typename base_ptr::const_reference;
  using const_reference = typename base_ptr::const_reference;
  // multimap 允许用通过其迭代器修改元素的实值 value_type
  using iterator = typename base_ptr::iterator;
  using const_iterator = typename base_ptr::const_iterator;
  using difference_type = typename base_ptr::difference_type;
  using size_type = typename base_ptr::size_type;

  multimap() : rb_tree(Compare()) {}
  explicit multimap(const Compare &comp) : rb_tree(comp) {}
  template<typename InputIterator>
  multimap(InputIterator first, InputIterator last) :rb_tree(Compare()) { rb_tree.insert_equal(first, last); }
  template<typename InputIterator>
  multimap(InputIterator first, InputIterator last, const Compare &comp) :rb_tree(comp) {
	rb_tree.insert_equal(first,
						  last);
  }
  explicit multimap(const map<Key, Compare, Alloc> &x) : rb_tree(x.rb_tree) {}
  multimap &operator=(const multimap &x) {
	rb_tree = x.rb_tree;
	return *this;
  }

  /* 以下为 multimap 所有操作，由于 <tree.h> 中的 RB-tree 实现均已提供，此处只做调用 */

  /* accessor 相关操作 */
  key_compare key_comp() const { return rb_tree.key_comp(); }
  value_compare value_comp() const { return key_comp(); }
  iterator begin() noexcept { return rb_tree.begin(); }
  const_iterator begin() const noexcept { return rb_tree.begin(); }
  iterator end() noexcept { return rb_tree.end(); }
  const_iterator end() const noexcept { return rb_tree.end(); }
  bool empty() const noexcept { return rb_tree.empty(); }
  size_type size() const noexcept { return rb_tree.size(); }
  size_type max_size() const noexcept { return rb_tree.max_size(); }
  void swap(map<Key, Compare, Alloc> &x) noexcept { rb_tree.swap(x.rb_tree); }

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
  iterator find(const key_type &x) { return rb_tree.find(x); }
  const_iterator find(const key_type &x) const { return rb_tree.find(x); }
  size_type count(const key_type &x) const { return rb_tree.count(x); }
  iterator lower_bound(const key_type &x) { return rb_tree.lower_bound(x); }
  const_iterator lower_bound(const key_type &x) const { return rb_tree.lower_bound(x); }
  iterator upper_bound(const key_type &x) { return rb_tree.upper_bound(x); }
  const_iterator upper_bound(const key_type &x) const { return rb_tree.upper_bound(x); }
  std::pair<iterator, iterator> equal_range(const key_type &x) { return rb_tree.equal_range(x); }
  std::pair<iterator, iterator> equal_range(const key_type &x) const { return rb_tree.equal_range(x); }
  friend bool operator==(const multimap &lhs, const multimap &rhs) { return lhs.rb_tree == rhs.rb_tree; }
  friend bool operator<(const multimap &lhs, const multimap &rhs) { return lhs.rb_tree < rhs.rb_tree; }
}; // class multimap end

/* 重载函数 */
template<typename Key, typename Compare, typename Allocator>
inline bool operator==(const multimap<Key, Compare, Allocator> &lhs, const multimap<Key, Compare, Allocator> &rhs) {
  return lhs == rhs;
}

template<typename Key, typename Compare, typename Allocator>
inline bool operator<(const multimap<Key, Compare, Allocator> &lhs, const multimap<Key, Compare, Allocator> &rhs) {
  return lhs < rhs;
}

template<typename Key, typename Compare, typename Allocator>
inline bool operator!=(const multimap<Key, Compare, Allocator> &lhs, const multimap<Key, Compare, Allocator> &rhs) {
  return !(lhs == rhs);
}

template<typename Key, typename Compare, typename Allocator>
inline bool operator>(const multimap<Key, Compare, Allocator> &lhs, const multimap<Key, Compare, Allocator> &rhs) {
  return rhs < lhs;
}

template<typename Key, typename Compare, typename Allocator>
inline bool operator<=(const multimap<Key, Compare, Allocator> &lhs, const multimap<Key, Compare, Allocator> &rhs) {
  return !(rhs < lhs);
}

template<typename Key, typename Compare, typename Allocator>
inline bool operator>=(const multimap<Key, Compare, Allocator> &lhs, const multimap<Key, Compare, Allocator> &rhs) {
  return !(lhs < rhs);
}

template<typename Key, typename Compare, typename Allocator>
inline void swap(const multimap<Key, Compare, Allocator> &lhs, const multimap<Key, Compare, Allocator> &rhs) noexcept {
  lhs.swap(rhs);
}

} // namespace tinystl

#endif //TINYSTL__MAP_H_
