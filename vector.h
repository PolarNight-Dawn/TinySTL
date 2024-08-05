//
// Created by polarnight on 24-8-2, 下午4:17.
//

#ifndef TINYSTL__VECTOR_H_
#define TINYSTL__VECTOR_H_

#include "memory.h"
#include "iterator.h"

namespace tinystl {
template<typename T, typename Allocator = Alloc>
class vector {
 public:
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using iterator = T *;
  using const_iterator = const T *;
  using reference = T &;
  using const_reference = const T &;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

 protected:
  using data_allocator = alloc<value_type, Alloc>;

  iterator start_;
  iterator finish_;
  iterator end_of_storage_;

  void insert_aux(iterator position, const_reference value);
  void deallocate() { if (start_) data_allocator::deallocate(start_, end_of_storage_ - start_); }
  void fill_init(size_type n, const_reference value);
  template<typename InputIterator>
  void copy_init(InputIterator first, InputIterator last);

 public:
  vector() : start_(0), finish_(0), end_of_storage_(0) {}
  vector(size_type n, const_reference value) { fill_init(n, value); }
  vector(int n, const_reference value) { fill_init(n, value); }
  vector(long n, const_reference value) { fill_init(n, value); }
  explicit vector(size_type n) { fill_init(n, T()); }
  explicit vector(const vector<T> &vec) { copy_init(vec.begin(), vec.end()); }
  template<typename InputIterator>
  vector(InputIterator first, InputIterator last) { copy_init(first, last); }
  vector(std::initializer_list<T> rhs) { copy_init(rhs.begin(), rhs.end()); }

  ~vector() {
	destroy(start_, finish_);
	deallocate();
  }

  vector<T, Allocator> &operator=(const vector<T, Allocator> &vec);
  vector<T, Allocator> &operator=(std::initializer_list<T> rhs);

  /* iterator 相关操作 */
  iterator begin() noexcept { return start_; }
  const_iterator begin() const noexcept { return start_; }
  iterator end() noexcept { return finish_; }
  const_iterator end() const noexcept { return finish_; }

  /* capacity 相关操作 */
  size_type size() const { return static_cast<size_type >(end() - begin()); }
  size_type max_size() const { return static_cast<size_type >(-1) / sizeof(T); }
  size_type capacity() const { return static_cast<size_type >(end_of_storage_ - begin()); }
  bool empty() const { return begin() == end(); }
  void reserve(size_type n);

  /* 访问相关操作 */
  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }
  reference back() { return *(end() - 1); } // [begin(), end())
  const_reference back() const { return *(end() - 1); }
  reference operator[](size_type n) { return *(begin() + n); }
  const_reference operator[](size_type n) const { return *(begin() + n); }

  /* container 相关操作 */
  void push_back(const T &value);
  void pop_back();
  void swap(vector<T, Allocator> &rhs);
  iterator insert(iterator position, const T &value);
  iterator insert(iterator position) { return insert(position, T()); }
  void insert(iterator position, size_type n, const T &value);
  iterator erase(iterator position);
  iterator erase(iterator first, iterator last);
  void resize(size_type new_size, const T &value);
  void resize(size_type new_size) { resize(new_size, T()); }
  void clear() { erase(start_, finish_); }
};

template<typename T, typename Allocator>
void vector<T, Allocator>::fill_init(size_type n, const T &value) {
  start_ = data_allocator::allocate(n);
  try {
	uninitialized_fill_n(start_, n, value);
	finish_ = start_ + n;
	end_of_storage_ = finish_;
  }
  catch (...) {
	data_allocator::deallocate(start_, n);
  }
}

template<typename T, typename Allocator>
template<typename InputIterator>
void vector<T, Allocator>::copy_init(InputIterator first, InputIterator last) {
  size_type n = last - first;
  start_ = data_allocator::allocate(n);
  try {
	uninitialized_copy(first, last, start_);
	finish_ = start_ + n;
	end_of_storage_ = finish_;
  }
  catch (...) {
	data_allocator::deallocate(start_, n);
	throw;
  }
}

template<typename T, typename Allocator>
void vector<T, Allocator>::insert_aux(iterator position, const T &value) {
  if (finish_ != end_of_storage_) {
	construct(finish_, *(finish_ - 1));
	++finish_;
	T x_copy = value;
	std::copy_backward(position, finish_ - 2, finish_ - 1);
	*position = x_copy;
  } else {
	const size_type old_size = size();
	const size_type new_size = old_size ? old_size * 2 : 1;
	iterator new_start = data_allocator::allocate(new_size);
	iterator new_finish = new_start;
	try {
	  new_finish = uninitialized_copy(start_, position, new_start);
	  construct(new_finish++, value);
	  new_finish = uninitialized_copy(position, finish_, new_finish);
	}
	catch (...) {
	  destroy(new_start, new_finish);
	  data_allocator::deallocate(new_start, new_size);
	  throw;
	}
	destroy(start_, finish_);
	deallocate();
	start_ = new_start;
	finish_ = new_finish;
	end_of_storage_ = start_ + new_size;
  }
}

/* vector 其余接口实现 */
// 等号操作符的重载
template<typename T, typename Allocator>
vector<T, Allocator> &vector<T, Allocator>::operator=(const vector<T, Allocator> &vec) {
  if (&vec != this) {
	size_type new_size = vec.size();
	if (new_size > capacity()) {
	  iterator new_start = data_allocator::allocate(new_size);
	  end_of_storage_ = uninitialized_copy(vec.begin(), vec.end(), new_start);
	  destroy(start_, finish_);
	  deallocate();
	  start_ = new_start;
	} else if (new_size < size()) {
	  iterator iter = std::copy(vec.begin(), vec.end(), start_);
	  destroy(iter, finish_);
	} else {
	  std::copy(vec.begin(), vec.begin() + size(), start_);
	  uninitialized_copy(vec.begin() + size(), vec.end(), finish_);
	}
	finish_ = start_ + new_size;
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
void vector<T, Allocator>::push_back(const T &value) {
  if (finish_ != end_of_storage_)
	construct(finish_++, value);
  else
	insert_aux(finish_, value);
}

template<typename T, typename Allocator>
void vector<T, Allocator>::pop_back() {
  --finish_;
  destroy(finish_);
}

template<typename T, typename Allocator>
void vector<T, Allocator>::swap(vector<T, Allocator> &rhs) {
  std::swap(start_, rhs.start_);
  std::swap(finish_, rhs.finish_);
  std::swap(end_of_storage_, rhs.end_of_storage_);
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::iterator vector<T, Allocator>::insert(iterator pos, const T &value) {
  size_type n = pos - start_;
  if (finish_ != end_of_storage_ && pos == finish_)
	construct(finish_++, value);
  else insert_aux(pos, value);
  return start_ + n;
}

template<typename T, typename Allocator>
void vector<T, Allocator>::insert(iterator pos, size_type n, const T &value) {
  if (n == 0) return;
  if (size() + n < capacity()) {
	const size_type elems_after = finish_ - pos;
	if (elems_after > n) {
	  uninitialized_copy(finish_ - n, finish_, finish_);
	  std::copy_backward(pos, finish_ - n, finish_);
	  std::fill(pos, pos + n, value);
	} else {
	  uninitialized_fill_n(finish_, n - elems_after, value);
	  uninitialized_copy(pos, finish_, pos + n);
	  std::fill(pos, finish_, value);
	}
	finish_ += n;
  } else {
	const size_type old_size = size();
	const size_type new_size = old_size + old_size > n ? old_size : n;
	iterator new_start = data_allocator::allocate(new_size);
	iterator new_finish = new_start;
	try {
	  new_finish = uninitialized_copy(start_, pos, new_start);
	  new_finish = uninitialized_fill_n(new_finish, n, value);
	  new_finish = uninitialized_copy(pos, finish_, new_finish);
	}
	catch (...) {
	  destroy(new_start, new_finish);
	  data_allocator::deallocate(new_start, new_size);
	  throw;
	}
	destroy(start_, finish_);
	deallocate();
	start_ = new_start;
	finish_ = new_finish;
	end_of_storage_ = new_start + new_size;
  }
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::iterator vector<T, Allocator>::erase(iterator pos) {
  if (pos != (finish_ - 1))
	std::copy(pos + 1, finish_, pos);
  destroy(finish_ - 1);
  --finish_;
  return pos;
}

template<typename T, typename Allocator>
typename vector<T, Allocator>::iterator vector<T, Allocator>::erase(iterator first, iterator last) {
  iterator new_finish = std::copy(last, finish_, first);
  destroy(new_finish, finish_);
  finish_ = new_finish;
  return first;
}

template<typename T, typename Allocator>
void vector<T, Allocator>::resize(size_type new_size, const T &value) {
  if (new_size < size())
	erase(start_ + new_size, finish_);
  else
	insert(finish_, new_size - size(), value);
}

template<typename T, typename Allocator>
void vector<T, Allocator>::reserve(size_type n) {
  if (capacity() < n) {
	iterator new_start = data_allocator::allocate(n);
	iterator new_finish = new_start;
	try {
	  new_finish = uninitialized_copy(start_, finish_, new_start);
	}
	catch (...) {
	  destroy(new_start, new_finish);
	  data_allocator::deallocate(new_start, n);
	}
	destroy(start_, finish_);
	deallocate();
	start_ = new_start;
	finish_ = new_finish;
	end_of_storage_ = start_ + n;
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
}

#endif //TINYSTL__VECTOR_H_
