//
// Created by polarnight on 24-8-2, 下午4:17.
//

#ifndef TINYSTL__VECTOR_H_
#define TINYSTL__VECTOR_H_

#include <bits/stl_vector.h>
#include "memory.h"
#include "iterator.h"

namespace tinystl {
template<typename T, typename Allocator = Alloc>
class vector {
 public:
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

  using iterator = T *;
  using const_iterator = const T *;
  using reverse_iterator = tinystl::reverse_iterator<iterator>;
  using const_reverse_iterator = tinystl::reverse_iterator<const_iterator>;

 protected:
  using data_allocator = alloc<value_type, Allocator>;

  iterator start;
  iterator finish;
  iterator end_of_storage;

  void destroy_and_recover(iterator first, iterator last, size_type n);
  size_type get_new_cap(size_type add_size);
  void insert_aux(iterator position, const value_type &value);
  void deallocate() { if (this->start) data_allocator::deallocate(this->start, this->end_of_storage - this->start); }
  void fill_init(size_type n, const value_type &value);
  template<typename InputIterator>
  void copy_init(InputIterator first, InputIterator last);
  template<typename ... Args>
  void reallocate_emplace(iterator pos, Args &&...args);

 public:
  vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr) {}
  explicit vector(size_type n) { fill_init(n, value_type()); }
  vector(size_type n, const value_type &value) { fill_init(n, value); }
  template<typename InputIterator>
  vector(InputIterator first, InputIterator last) { copy_init(first, last); }
  explicit vector(const vector<value_type> &rhs) { copy_init(rhs.begin(), rhs.end()); }
  vector(std::initializer_list<value_type> rhs) { copy_init(rhs.begin(), rhs.end()); }

  ~vector() {
	destroy(this->start, this->finish);
	deallocate();
  }

  vector &operator=(const vector &rhs);
  vector &operator=(vector &&rhs) noexcept;
  vector &operator=(std::initializer_list<value_type> rhs);

  /* iterator 相关操作 */
  iterator begin() noexcept { return this->start; }
  const_iterator begin() const noexcept { return this->start; }
  iterator end() noexcept { return this->finish; }
  const_iterator end() const noexcept { return this->finish; }
  reverse_iterator rbegin() noexcept { return static_cast<reverse_iterator>(end()); }
  const_reverse_iterator rbegin() const noexcept { return static_cast<const_reverse_iterator>(end()); }
  reverse_iterator rend() noexcept { return static_cast<reverse_iterator>(begin()); }
  const_reverse_iterator rend() const noexcept { return static_cast<const_reverse_iterator>(begin()); }

  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }

  /* capacity 相关操作 */
  size_type size() const noexcept { return static_cast<size_type >(end() - begin()); }
  size_type max_size() const noexcept { return static_cast<size_type >(-1) / sizeof(T); }
  size_type capacity() const noexcept { return static_cast<size_type >(this->end_of_storage - begin()); }
  bool empty() const noexcept { return begin() == end(); }
  void reserve(size_type n);

  /* access 相关操作 */
  reference front() noexcept { return *begin(); }
  const_reference front() const noexcept { return *begin(); }
  reference back() noexcept { return *(end() - 1); } // [begin(), end())
  const_reference back() const noexcept { return *(end() - 1); }
  reference operator[](size_type n) noexcept { return *(begin() + n); }
  const_reference operator[](size_type n) const noexcept { return *(begin() + n); }

  /* container 相关操作 */
  template<typename ... Args>
  iterator emplace(const_iterator pos, Args &&...args);
  template<typename ... Args>
  void emplace_back(Args &&...args);
  void push_back(const value_type &value);
  void push_back(value_type &&value) { emplace_back(std::move(value)); }
  void pop_back();
  void swap(vector<value_type, Allocator> &rhs);
  iterator insert(iterator position, const value_type &value);
  iterator insert(iterator position) { return insert(position, value_type()); }
  iterator insert(const_iterator pos, value_type &&value) { return emplace(pos, std::move(value)); }
  void insert(iterator position, size_type n, const value_type &value);
  iterator erase(iterator position);
  iterator erase(iterator first, iterator last);
  void resize(size_type new_size, const value_type &value);
  void resize(size_type new_size) { resize(new_size, value_type()); }
  void clear() { erase(this->start, this->finish); }
};

template<typename T, typename Allocator>
void vector<T, Allocator>::fill_init(size_type n, const T &value) {
  this->start = data_allocator::allocate(n);
  try {
	uninitialized_fill_n(this->start, n, value);
	this->finish = this->start + n;
	this->end_of_storage = this->finish;
  }
  catch (...) {
	data_allocator::deallocate(this->start, n);
  }
}

template<typename T, typename Allocator>
template<typename InputIterator>
void vector<T, Allocator>::copy_init(InputIterator first, InputIterator last) {
  size_type n = last - first;
  this->start = data_allocator::allocate(n);
  try {
	uninitialized_copy(first, last, this->start);
	this->finish = this->start + n;
	this->end_of_storage = this->finish;
  }
  catch (...) {
	data_allocator::deallocate(this->start, n);
	throw;
  }
}

template<typename T, typename Allocator>
void vector<T, Allocator>::destroy_and_recover(iterator first, iterator last, size_type n) {
  data_allocator::destroy(first, last);
  data_allocator::deallocate(first, n);
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::size_type vector<T, Allocator>::get_new_cap(size_type add_size) {
  const auto old_size = capacity();
  if (old_size > max_size() - old_size / 2)
	return old_size + add_size > max_size() - 16 ? old_size + add_size : old_size + add_size + 16;
  const size_type new_size =
	  old_size == 0 ? std::max(add_size, static_cast<size_type>(16)) : std::max(old_size + old_size / 2,
																				old_size + add_size);
  return new_size;
}

template<typename T, typename Allocator>
void vector<T, Allocator>::insert_aux(iterator position, const T &value) {
  if (this->finish != this->end_of_storage) {
	construct(this->finish, *(this->finish - 1));
	++this->finish;
	T x_copy = value;
	std::copy_backward(position, this->finish - 2, this->finish - 1);
	*position = x_copy;
  } else {
	const size_type old_size = size();
	const size_type new_size = old_size ? old_size * 2 : 1;
	iterator new_start = data_allocator::allocate(new_size);
	iterator new_finish = new_start;
	try {
	  new_finish = uninitialized_copy(this->start, position, new_start);
	  construct(new_finish++, value);
	  new_finish = uninitialized_copy(position, this->finish, new_finish);
	}
	catch (...) {
	  destroy(new_start, new_finish);
	  data_allocator::deallocate(new_start, new_size);
	  throw;
	}
	destroy(this->start, this->finish);
	deallocate();
	this->start = new_start;
	this->finish = new_finish;
	this->end_of_storage = this->start + new_size;
  }
}

template<typename T, typename Allocator>
template<class ...Args>
void vector<T, Allocator>::
reallocate_emplace(iterator pos, Args &&...args) {
  const auto new_size = get_new_cap(1);
  auto new_begin = data_allocator::allocate(new_size);
  auto new_end = new_begin;
  try {
	new_end = std::uninitialized_move(start, pos, new_begin);
	data_allocator::construct(std::addressof(*new_end), std::forward<Args>(args)...);
	++new_end;
	new_end = std::uninitialized_move(pos, finish, new_end);
  }
  catch (...) {
	data_allocator::deallocate(new_begin, new_size);
	throw;
  }
  destroy_and_recover(start, finish, end_of_storage - start);
  start = new_begin;
  finish = new_end;
  end_of_storage = new_begin + new_size;
}

/* vector 其余接口实现 */
// 等号操作符的重载
template<typename T, typename Allocator>
vector<T, Allocator> &vector<T, Allocator>::operator=(const vector<T, Allocator> &rhs) {
  if (&rhs != this) {
	size_type new_size = rhs.size();
	if (new_size > capacity()) {
	  iterator new_start = data_allocator::allocate(new_size);
	  this->end_of_storage = uninitialized_copy(rhs.begin(), rhs.end(), new_start);
	  destroy(this->start, this->finish);
	  deallocate();
	  this->start = new_start;
	} else if (new_size < size()) {
	  iterator iter = std::copy(rhs.begin(), rhs.end(), this->start);
	  destroy(iter, this->finish);
	} else {
	  std::copy(rhs.begin(), rhs.begin() + size(), this->start);
	  uninitialized_copy(rhs.begin() + size(), rhs.end(), this->finish);
	}
	this->finish = this->start + new_size;
  }
  return *this;
}

template<typename T, typename Allocator>
vector<T, Allocator> &vector<T, Allocator>::operator=(std::initializer_list<T> rhs) {
  vector<T, Allocator> tmp(rhs.begin(), rhs.end());
  swap(tmp);
  return *this;
}

template<typename T, typename Allocator>
template<class ...Args>
typename vector<T, Allocator>::iterator
vector<T, Allocator>::emplace(const_iterator pos, Args &&...args) {
  iterator xpos = const_cast<iterator>(pos);
  const size_type n = xpos - this->begin;
  if (this->finish != this->end_of_storage && xpos == this->finish) {
	data_allocator::construct(std::addressof(*this->finish), std::forward<Args>(args)...);
	++this->finish;
  } else if (this->finish != this->end_of_storage) {
	auto new_end = this->finish;
	data_allocator::construct(std::addressof(*this->finish), *(this->finish - 1));
	++new_end;
	std::copy_backward(xpos, this->finish - 1, this->finish);
	*xpos = value_type(std::forward<Args>(args)...);
	this->finish = new_end;
  } else {
	reallocate_emplace(xpos, std::forward<Args>(args)...);
  }
  return begin() + n;
}

template<typename T, typename Allocator>
template<class ...Args>
void vector<T, Allocator>::emplace_back(Args &&...args) {
  if (this->finish < this->end_of_storage) {
	data_allocator::construct(std::addressof(*this->finish), std::forward<Args>(args)...);
	++this->finish;
  } else {
	reallocate_emplace(this->finish, std::forward<Args>(args)...);
  }
}

template<typename T, typename Allocator>
void vector<T, Allocator>::push_back(const T &value) {
  if (this->finish != this->end_of_storage)
	construct(this->finish++, value);
  else
	insert_aux(this->finish, value);
}

template<typename T, typename Allocator>
void vector<T, Allocator>::pop_back() {
  --this->finish;
  destroy(this->finish);
}

template<typename T, typename Allocator>
void vector<T, Allocator>::swap(vector<T, Allocator> &rhs) {
  std::swap(this->start, rhs.start);
  std::swap(this->finish, rhs.finish);
  std::swap(this->end_of_storage, rhs.end_of_storage);
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::iterator vector<T, Allocator>::insert(iterator pos, const T &value) {
  size_type n = pos - this->start;
  if (this->finish != this->end_of_storage && pos == this->finish)
	construct(this->finish++, value);
  else insert_aux(pos, value);
  return this->start + n;
}

template<typename T, typename Allocator>
void vector<T, Allocator>::insert(iterator pos, size_type n, const T &value) {
  if (n == 0) return;
  if (size() + n < capacity()) {
	const size_type elems_after = this->finish - pos;
	if (elems_after > n) {
	  uninitialized_copy(this->finish - n, this->finish, this->finish);
	  std::copy_backward(pos, this->finish - n, this->finish);
	  std::fill(pos, pos + n, value);
	} else {
	  uninitialized_fill_n(this->finish, n - elems_after, value);
	  uninitialized_copy(pos, this->finish, pos + n);
	  std::fill(pos, this->finish, value);
	}
	this->finish += n;
  } else {
	const size_type old_size = size();
	const size_type new_size = old_size + old_size > n ? old_size : n;
	iterator new_start = data_allocator::allocate(new_size);
	iterator new_finish = new_start;
	try {
	  new_finish = uninitialized_copy(this->start, pos, new_start);
	  new_finish = uninitialized_fill_n(new_finish, n, value);
	  new_finish = uninitialized_copy(pos, this->finish, new_finish);
	}
	catch (...) {
	  destroy(new_start, new_finish);
	  data_allocator::deallocate(new_start, new_size);
	  throw;
	}
	destroy(this->start, this->finish);
	deallocate();
	this->start = new_start;
	this->finish = new_finish;
	this->end_of_storage = new_start + new_size;
  }
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::iterator vector<T, Allocator>::erase(iterator pos) {
  if (pos != (this->finish - 1))
	std::copy(pos + 1, this->finish, pos);
  destroy(this->finish - 1);
  --this->finish;
  return pos;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::iterator vector<T, Allocator>::erase(iterator first, iterator last) {
  iterator new_finish = std::copy(last, this->finish, first);
  destroy(new_finish, this->finish);
  this->finish = new_finish;
  return first;
}

template<typename T, typename Allocator>
void vector<T, Allocator>::resize(size_type new_size, const T &value) {
  if (new_size < size())
	erase(this->start + new_size, this->finish);
  else
	insert(this->finish, new_size - size(), value);
}

template<typename T, typename Allocator>
void vector<T, Allocator>::reserve(size_type n) {
  if (capacity() < n) {
	iterator new_start = data_allocator::allocate(n);
	iterator new_finish = new_start;
	try {
	  new_finish = uninitialized_copy(this->start, this->finish, new_start);
	}
	catch (...) {
	  destroy(new_start, new_finish);
	  data_allocator::deallocate(new_start, n);
	}
	destroy(this->start, this->finish);
	deallocate();
	this->start = new_start;
	this->finish = new_finish;
	this->end_of_storage = this->start + n;
  }
}

template<typename T, typename Allocator>
inline bool operator==(const vector<T, Allocator> &lhs, const vector<T, Allocator> &rhs) {
  return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, typename Allocator>
inline bool operator<(const vector<T, Allocator> &lhs, const vector<T, Allocator> &rhs) {
  typename vector<T, Allocator>::iterator first1 = lhs.begin();
  auto last1 = lhs.end();
  auto first2 = rhs.begin();
  auto last2 = rhs.end();
  for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
	if (*first1 < *first2) return true;
	if (*first2 < *first1) return false;
  }
  return first1 == last1 && first2 != last2;
}

template<typename T, typename Allocator>
inline void swap(vector<T, Allocator> &lhs, vector<T, Allocator> &rhs) {
  lhs.swap(rhs);
}

} // namespace tinystl

#endif //TINYSTL__VECTOR_H_
