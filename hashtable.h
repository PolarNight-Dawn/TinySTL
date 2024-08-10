//
// Created by polarnight on 24-8-10, 下午3:36.
//

#ifndef TINYSTL__HASHTABLE_H_
#define TINYSTL__HASHTABLE_H_

/* <hashtable.h> 以开链法完成 hashtable */

#include "memory.h"
#include "vector.h"

namespace tinystl {
template<typename Value>
struct hashtable_node {
  hashtable_node *next;
  Value val;
};

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator = Alloc>
class hashtable;

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator = Alloc>
struct hashtable_iterator;

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator = Alloc>
struct hashtable_const_iterator;

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
struct hashtable_iterator {
  using hashtable = hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>;
  using iterator = hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>;
  using const_iterator = hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>;
  using node = hashtable_node<Value>;

  using iterator_category = forward_iterator_tag;
  using value_type = Value;
  using difference_type = ptrdiff_t;
  using size_type = size_t;
  using reference = Value &;
  using pointer = Value *;

  node *cur;
  hashtable *hash_table;

  hashtable_iterator(node *n, hashtable *tab) : cur(n), hash_table(tab) {}
  hashtable_iterator() = default;
  reference operator*() const { return cur->val; }
  pointer operator->() const { return &(operator*()); }
  iterator &operator++();
  iterator operator++(int);
  bool operator==(const iterator &rhs) const { return cur == rhs.cur; }
  bool operator!=(const iterator &rhs) const { return cur != rhs.cur; }
};

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator> &
hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::operator++() {
  const node *old = cur;
  cur = cur->next;
  if (!cur) {
	size_type bucket = hash_table->bkt_num(old->val);
	while (!cur && ++bucket < hash_table->buckets.size())
	  cur = hash_table->buckets[bucket];
  }
  return *this;
}

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>
hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::operator++(int) {
  iterator tmp = *this;
  ++*this;
  return tmp;
}

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
struct hashtable_const_iterator {
  using hashtable = hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>;
  using iterator = hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>;
  using const_iterator = hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>;
  using node = hashtable_node<Value>;

  using iterator_category = forward_iterator_tag;
  using value_type = Value;
  using difference_type = ptrdiff_t;
  using size_type = size_t;
  using reference = const Value &;
  using pointer = const Value *;

  const node *cur;
  const hashtable *hash_table;

  hashtable_const_iterator(node *n, hashtable *tab) : cur(n), hash_table(tab) {}
  hashtable_const_iterator() = default;
  reference operator*() const { return cur->val; }
  pointer operator->() const { return &(operator*()); }
  const_iterator &operator++();
  const_iterator operator++(int);
  bool operator==(const iterator &rhs) const { return cur == rhs.cur; }
  bool operator!=(const iterator &rhs) const { return cur != rhs.cur; }
};

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator> &
hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::operator++() {
  const node *old = cur;
  cur = cur->next;
  if (!cur) {
	size_type bucket = hash_table->bkt_num(old->val);
	while (!cur && ++bucket < hash_table->buckets.size())
	  cur = hash_table->buckets[bucket];
  }
  return *this;
}

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>
hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::operator++(int) {
  iterator tmp = *this;
  ++*this;
  return tmp;
}

static const int tinystl_num_primes = 28;
static const unsigned long tinystl_prime_list[tinystl_num_primes] = {
	53ul, 97ul, 193ul, 389ul, 769ul,
	1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
	49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
	1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
	50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
	1610612741ul, 3221225473ul, 4294967291ul
};

inline unsigned long tinystl_next_prime(unsigned long n) {
  const unsigned long *first = tinystl_prime_list;
  const unsigned long *last = tinystl_prime_list + tinystl_num_primes;
  const unsigned long *pos = std::lower_bound(first, last, n);
  return pos == last ? *(last - 1) : *pos;
}

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
class hashtable {
 public:
  using value_type = Value;
  using key_type = Key;
  using hasher = HashFcn;
  using key_equal = EqualKey;

  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using reference = value_type &;
  using const_reference = const value_type &;
  using iterator = hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>;
  using const_iterator = hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>;

 private:
  hasher hash;
  key_equal equals;
  ExtractKey get_key;

  using node = hashtable_node<Value>;
  using node_allocator = alloc<node, Allocator>;
  using data_allocator = alloc<node, Allocator>;

 public:
  vector<node *, Allocator> buckets;
  size_type num_elements;

  friend struct hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>;
  friend struct hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>;

 public:
  hashtable(size_type n, const HashFcn &hf, const EqualKey &eql, const ExtractKey &ext)
	  : hash(hf), equals(eql), get_key(ext), num_elements(0) {
	init_buckets(n);
  }
  hashtable(size_type n, const HashFcn &hf, const EqualKey &eql)
	  : hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0) {
	init_buckets(n);
  }
  hashtable(const hashtable &rhs)
	  : hash(rhs.hash), equals(rhs.equals), get_key(rhs.get_key), num_elements(0) {
	copy_from(rhs);
  }
  ~hashtable() { clear(); }

  hashtable &operator=(const hashtable &ht) {
	if (&ht != this) {
	  clear();
	  hash = ht.hash;
	  equals = ht.equals;
	  get_key = ht.get_key;
	  copy_from(ht);
	}
	return *this;
  }

  iterator begin() noexcept {
	for (size_type n = 0; n < buckets.size(); ++n)
	  if (buckets[n]) return iterator(buckets[n], this);
	return end();
  }
  iterator begin() const noexcept {
	for (size_type n = 0; n < buckets.size(); ++n)
	  if (buckets[n]) return iterator(buckets[n], this);
	return end();
  }
  iterator end() noexcept { return static_cast<iterator>(nullptr, this); }
  iterator end() const noexcept { return static_cast<iterator>(nullptr, this); }

  size_type size() const { return num_elements; }
  size_type max_size() const { return size_type(-1); }
  bool empty() const { return size() == 0; }
  void swap(hashtable &rhs) {
	buckets.swap(rhs.buckets);
	std::swap(hash, rhs.hash);
	std::swap(equals, rhs.equals);
	std::swap(get_key, rhs.get_key);
	std::swap(num_elements, rhs.num_elements);
  }
  size_type bucket_count() const { return buckets.size(); }
  size_type max_bucket_count() const { return tinystl_prime_list[tinystl_num_primes - 1]; }
  size_type elems_in_bucket(size_type n) const {
	size_type result = 0;
	for (node *cur = buckets[n]; cur; cur = cur->next)
	  result += 1;
	return result;
  }

  // 不允许有重复的节点
  std::pair<iterator, bool> insert_unique(const value_type &obj) {
	resize(num_elements + 1); // 判断是否需要重置 buckets
	return insert_unique_noresize(obj);
  }
  // 允许有重复的节点
  iterator insert_equal(const value_type &obj) {
	resize(num_elements + 1);
	return insert_equal_noresize(obj);
  }
  std::pair<iterator, bool> insert_unique_noresize(const value_type &obj);
  iterator insert_equal_noresize(const value_type &obj);
  template<typename InputIterator>
  void insert_unique(InputIterator first, InputIterator last) {
	insert_unique(first, last, __ITERATOR_CATEGORY(first));
  }
  template<typename InputIterator>
  void insert_equal(InputIterator first, InputIterator last) {
	insert_equal(first, last, __ITERATOR_CATEGORY(first));
  }
  template<typename InputIterator>
  void insert_unique(InputIterator first, InputIterator last, input_iterator_tag) {
	for (; first != last; ++first)
	  insert_unique(*first);
  }
  template<typename InputIterator>
  void insert_equal(InputIterator first, InputIterator last, input_iterator_tag) {
	for (; first != last; ++first)
	  insert_equal(*first);
  }
  template<typename ForwardIterator>
  void insert_unique(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
	size_type n = 0;
	distance(first, last, n);
	resize(num_elements + n);
	for (; n > 0; --n, ++first)
	  insert_unique_noresize(*first);
  }
  template<typename ForwardIterator>
  void insert_equal(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
	size_type n = 0;
	distance(first, last, n);
	resize(num_elements + n);
	for (; n > 0; --n, ++first)
	  insert_equal_noresize(*first);
  }
  std::pair<iterator, iterator> equal_range(const key_type &key);
  std::pair<const_iterator, const_iterator> equal_range(const key_type &key) const;
  size_type erase(const key_type &key);
  void erase(const const_iterator &rhs);
  void erase(const_iterator first, const_iterator last);
  void resize(size_type n);
  void clear();

  reference find_or_insert(const value_type &obj);
  iterator find(const key_type &key) {
	size_type n = bkt_num_key(key);
	node *first;
	for (first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next) {}
	return static_cast<iterator>(first, this);
  }
  const_iterator find(const key_type &key) const {
	size_type n = bkt_num_key(key);
	const node *first;
	for (first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next) {}
	return static_cast<const_iterator>(first, this);
  }
  size_type count(const key_type &key) const {
	const size_type n = bkt_num_key(key);
	size_type result = 0;

	for (const node *cur = buckets[n]; cur; cur = cur->next)
	  if (equals(get_key(cur->val), key))++result;
	return result;
  }

 private:
  size_type next_size(size_type n) const { return tinystl_next_prime(n); }
  // 初始化 buckets vector
  void init_buckets(size_type n) {
	const size_type n_buckets = next_size(n);
	buckets.reserve(n_buckets);
	buckets.insert(buckets.end(), n_buckets, static_cast<node *>(nullptr));
	num_elements = 0;
  }
  // 只接受键值
  size_type bkt_num_key(const key_type &key) const { return bkt_num_key(key, buckets.size()); }
  // 只接受实值
  size_type bkt_num(const value_type &obj) const { return bkt_num_key(get_key(obj)); }
  // 接受键值和 buckets 个数
  size_type bkt_num_key(const key_type &key, size_t n) const { return hash(key) % n; }
  // 接受实值和 buckets 个数
  size_type bkt_num(const value_type &obj, size_t n) const { return bkt_num_key(get_key(obj), n); }

  // 节点配置函数
  node *new_node(const value_type &obj) {
	node *tmp = node_allocator::allocate(1);
	try {
	  data_allocator::construct(obj);
	  tmp->next = nullptr;
	} catch (...) {
	  node_allocator::deallocate(tmp);
	  throw;
	}
	return tmp;
  }
  // 节点释放函数
  void delete_node(node *n) {
	data_allocator::destroy(&n->val);
	node_allocator::deallocate(n);
	n = nullptr;
  }

  void erase_bucket(const size_type n, node *first, node *last);
  void erase_bucket(const size_type n, node *last);
  void copy_from(const hashtable &rhs);
};

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
inline forward_iterator_tag
iterator_category(const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator> &) {
  return forward_iterator_tag();
}

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
inline Value *value_type(const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator> &) {
  return static_cast<Value *>(nullptr);
}

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
inline hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::difference_type *
distance_type(const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator> &) {
  return static_cast<hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::difference_type *>(nullptr);
}

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
inline forward_iterator_tag
iterator_category(const hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator> &) {
  return forward_iterator_tag();
}

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
inline Value *
value_type(const hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator> &) {
  return static_cast<Value *>(nullptr);
}

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
inline hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::difference_type *
distance_type(const hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator> &) {
  return static_cast<hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::difference_type *>(nullptr);
}

// operator== 操作，vector 和 list 
template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
bool operator==(const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator> &__ht1,
				const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator> &__ht2) {
  typedef typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::node node;
  if (__ht1.buckets.size() != __ht2.buckets.size())
	return false;
  for (int n = 0; n < __ht1.buckets.size(); ++n) {
	node *__cur1 = __ht1.buckets[n];
	node *__cur2 = __ht2.buckets[n];
	for (; __cur1 && __cur2 && __cur1->val == __cur2->val;
		   __cur1 = __cur1->next, __cur2 = __cur2->next) {}
	if (__cur1 || __cur2)
	  return false;
  }
  return true;
}

// 在不需要重建 buckets 大小下，插入新节点，键值不能重复
template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
std::pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::iterator, bool>
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>
::insert_unique_noresize(const value_type &obj) {
  const size_type n = bkt_num(obj);
  node *first = buckets[n];

  for (node *cur = first; cur; cur = cur->next)
	if (_M_equals(get_key(cur->val), get_key(obj)))
	  return std::pair<iterator, bool>(iterator(cur, this), false);  // 说明插入节点已经在 hash table 中，不用插入

  node *__tmp = new_node(obj); // 头插法
  __tmp->next = first;
  buckets[n] = __tmp;
  ++num_elements;
  return std::pair<iterator, bool>(iterator(__tmp, this), true);
}

// 在不需要重建 buckets 大小下，插入新节点，键值可以重复
template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::iterator
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>
::insert_equal_noresize(const value_type &obj) {
  const size_type n = bkt_num(obj);
  node *first = buckets[n];

  for (node *cur = first; cur; cur = cur->next)
	if (_M_equals(get_key(cur->val), get_key(obj))) {
	  node *__tmp = new_node(obj); // 相等，插入后面
	  __tmp->next = cur->next;
	  cur->next = __tmp;
	  ++num_elements;
	  return iterator(__tmp, this);
	}

  node *__tmp = new_node(obj);
  __tmp->next = first;
  buckets[n] = __tmp;
  ++num_elements;
  return iterator(__tmp, this);
}

// 先查找要插入节点是否在hash table 中，有返回，没有插入新节点
template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::reference
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::find_or_insert(const value_type &obj) {
  resize(num_elements + 1);

  size_type n = bkt_num(obj);
  node *first = buckets[n];

  for (node *cur = first; cur; cur = cur->next)
	if (_M_equals(get_key(cur->val), get_key(obj)))return cur->val;

  node *__tmp = new_node(obj);
  __tmp->next = first;
  buckets[n] = __tmp;
  ++num_elements;
  return __tmp->val;
}

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
std::pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::iterator,
		  typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::iterator>
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::equal_range(const key_type &key) {
  typedef std::pair<iterator, iterator> _Pii;
  const size_type n = bkt_num_key(key);

  for (node *first = buckets[n]; first; first = first->next)
	if (_M_equals(get_key(first->val), key)) {
	  for (node *cur = first->next; cur; cur = cur->next)
		if (!_M_equals(get_key(cur->val), key))return _Pii(iterator(first, this), iterator(cur, this));
	  for (size_type __m = n + 1; __m < buckets.size(); ++__m)
		if (buckets[__m])return _Pii(iterator(first, this), iterator(buckets[__m], this));
	  return _Pii(iterator(first, this), end());
	}
  return _Pii(end(), end());
}

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
std::pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::const_iterator,
		  typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::const_iterator>
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>
::equal_range(const key_type &key) const {
  typedef std::pair<const_iterator, const_iterator> _Pii;
  const size_type n = bkt_num_key(key);

  for (const node *first = buckets[n];
	   first;
	   first = first->next) {
	if (_M_equals(get_key(first->val), key)) {
	  for (const node *cur = first->next;
		   cur;
		   cur = cur->next)
		if (!_M_equals(get_key(cur->val), key))
		  return _Pii(const_iterator(first, this),
					  const_iterator(cur, this));
	  for (size_type __m = n + 1; __m < buckets.size(); ++__m)
		if (buckets[__m])
		  return _Pii(const_iterator(first, this),
					  const_iterator(buckets[__m], this));
	  return _Pii(const_iterator(first, this), end());
	}
  }
  return _Pii(end(), end());
}

// 删除指定的节点
template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::size_type
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::erase(const key_type &key) {
  const size_type n = bkt_num_key(key);
  node *first = buckets[n];
  size_type __erased = 0;

  if (first) {
	node *cur = first;
	node *__next = cur->next;
	while (__next) {
	  if (_M_equals(get_key(__next->val), key)) {
		cur->next = __next->next;
		delete_node(__next);
		__next = cur->next;
		++__erased;
		--num_elements;
	  } else {
		cur = __next;
		__next = cur->next;
	  }
	}
	if (_M_equals(get_key(first->val), key)) {
	  buckets[n] = first->next;
	  delete_node(first);
	  ++__erased;
	  --num_elements;
	}
  }
  return __erased;
}

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
inline void
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::erase(const_iterator first, const_iterator __last) {
  erase(iterator(const_cast<node *>(first._M_cur), const_cast<hashtable *>(first._M_ht)),
		iterator(const_cast<node *>(__last._M_cur), const_cast<hashtable *>(__last._M_ht)));
}

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
inline void
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::erase(const const_iterator &rhs) {
  erase(iterator(const_cast<node *>(rhs._M_cur), const_cast<hashtable *>(rhs._M_ht)));
}

// 判断重建 buckets 大小
template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::resize(size_type __num_elements_hint) {
  const size_type __old_n = buckets.size();
  if (__num_elements_hint > __old_n) {
	const size_type n = next_size(__num_elements_hint); // 找到下一个质数
	if (n > __old_n) {
	  vector<node *, Allocator> __tmp(n, (node *)(0), buckets.get_allocator()); // 设置新的 buckets
	  try {
		for (size_type __bucket = 0; __bucket < __old_n; ++__bucket) {
		  node *first = buckets[__bucket];
		  while (first) {
			size_type __new_bucket = bkt_num(first->val, n);
			buckets[__bucket] = first->next;  // buckets 旧
			first->next = __tmp[__new_bucket]; // __tmp 新
			__tmp[__new_bucket] = first;
			first = buckets[__bucket];
		  }
		}
		buckets.swap(__tmp); // 新旧对调
	  } catch (...) {
		for (size_type __bucket = 0; __bucket < __tmp.size(); ++__bucket) {
		  while (__tmp[__bucket]) {
			node *__next = __tmp[__bucket]->next;
			delete_node(__tmp[__bucket]);
			__tmp[__bucket] = __next;
		  }
		}
		throw;
	  }
	}
  }
}

// 删除指定 bucket 下，[first, last) 范围下的节点
template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>
::erase_bucket(const size_type n, node *first, node *__last) {
  node *cur = buckets[n];
  if (cur == first)
	_M_erase_bucket(n, __last);
  else {
	node *__next;
	for (__next = cur->next;
		 __next != first;
		 cur = __next, __next = cur->next);
	while (__next != __last) {
	  cur->next = __next->next;
	  delete_node(__next);
	  __next = cur->next;
	  --num_elements;
	}
  }
}

// 删除指定的 bucket 下所有节点
template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::erase_bucket(const size_type n, node *__last) {
  node *cur = buckets[n];
  while (cur != __last) {
	node *__next = cur->next;
	delete_node(cur);
	cur = __next;
	buckets[n] = cur;
	--num_elements;
  }
}

// 清空所有节点
template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::clear() {
  for (size_type __i = 0; __i < buckets.size(); ++__i) {
	node *cur = buckets[__i];
	while (cur != 0) {
	  node *__next = cur->next;
	  delete_node(cur);
	  cur = __next;
	}
	buckets[__i] = 0;
  }
  num_elements = 0;
}

// 哈希表复制，第一：vector 复制，第二：linked list 复制   
template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>::copy_from(const hashtable &ht) {
  buckets.clear();
  buckets.reserve(ht.buckets.size());
  buckets.insert(buckets.end(), ht.buckets.size(), static_cast<node *>(nullptr));
  try {
	for (size_type __i = 0; __i < ht.buckets.size(); ++__i) {
	  const node *cur = ht.buckets[__i];
	  if (cur) {
		node *copy = new_node(cur->val);
		buckets[__i] = copy;

		for (node *__next = cur->next; __next; cur = __next, __next = cur->next) {
		  copy->next = new_node(__next->val);
		  copy = copy->next;
		}
	  }
	}
	num_elements = ht.num_elements;
  } catch (...) {

  }
  clear();
}

} // namespace tinystl

#endif //TINYSTL__HASHTABLE_H_
