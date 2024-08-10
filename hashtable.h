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

template<typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Allocator>
struct hashtable_iterator {
  using hashtable = hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>;
  using iterator = hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>;
  using const_iterator = hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Allocator>;
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

static const int tinystl_num_primes = 28;
static const unsigned long tinystl_prime_list[tinystl_num_primes] = {
	53, 97, 193, 389, 769,
	1543, 3079, 6151, 12289, 24593,
	49157, 98317, 196613, 393241, 786433,
	1572869, 3145739, 6291429, 12582917, 25165843,
	50331653, 100663319, 201326611, 402653189, 805306457,
	1610612741, 3221225473ul, 429496729ul
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
  using hasher = HashFcn;
  using key_equal = EqualKey;
  using size_type = size_t;

 private:
  hasher hash;
  key_equal equals;
  ExtractKey get_key;

  using node = hashtable_node<Value>;
  using node_allocator = alloc<node, Allocator>;

  vector<node *, Allocator> buckets;
  size_type num_elements;

 public:
  node* new_node(const value_type&obj)

 public:


 public:
  size_type bucket_count() const { return buckets.size(); }
  size_type max_bucket_count() const { return tinystl_prime_list[tinystl_num_primes - 1]; }

};

}

#endif //TINYSTL__HASHTABLE_H_
