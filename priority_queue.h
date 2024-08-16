//
// Created by polarnight on 24-8-16, 上午11:23.
//

#ifndef TINYSTL__PRIORITY_QUEUE_H_
#define TINYSTL__PRIORITY_QUEUE_H_

/* <priority_queue.h> 实现了 priority_queue
 * 此处以 vector 为底部结构并封闭其头端开口，就能轻松实现 */

#include "vector.h"
#include "heap.h"

namespace tinystl {
template<typename T, typename Sequence = tinystl::vector<T>,
	typename Compare = std::less<typename Sequence::value_type>>
class priority_queue {
 public:
  using value_type = typename Sequence::value_type;
  using size_type = typename Sequence::size_type;
  using reference = typename Sequence::reference;
  using const_reference = typename Sequence::const_reference;

 private:
  Sequence c;
  Compare comp;
 public:
  priority_queue() = default;
  explicit priority_queue(const Compare &x) : c(), comp(x) {}
  template<typename InputIterator>
  priority_queue(InputIterator first, InputIterator last) : c(first, last) {
	tinystl::make_heap(c.begin(), c.end(), comp);
  }
  template<typename InputIterator>
  priority_queue(InputIterator first, InputIterator last, const Compare &x) : c(first, last), comp(x) {
	tinystl::make_heap(c.begin(), c.end(), comp);
  }

 public:
  bool empty() const noexcept { return c.empty(); }
  size_type size() const noexcept { return c.size(); }
  const_reference top() const { return c.front(); }
  void push(const value_type &value) {
	try {
	  c.push_back(value);
	  tinystl::push_heap(c.begin(), c.end(), comp);
	} catch (...) {
	  c.clear();
	}
  }
  void pop() {
	try {
	  tinystl::pop_heap(c.begin(), c.end(), comp);
	  c.pop_back();
	} catch (...) {
	  c.clear();
	}
  }
};

}  // namespace tinystl

#endif //TINYSTL__PRIORITY_QUEUE_H_
