//
// Created by polarnight on 24-8-16, 上午10:12.
//

#ifndef TINYSTL__QUEUE_H_
#define TINYSTL__QUEUE_H_

/* <queue.h> 实现了 queue
 * 此处以 deque 为底部结构并封闭其头端开口，就能轻松实现
 * 但是同理，以 list 为底部结构并封闭其头端开口也可以，尚待实现 */

#include "deque.h"

namespace tinystl {
template<typename T, typename Container = tinystl::deque<T>>
class queue {
 public:
  using value_type = typename Container::value_type;
  using size_type = typename Container::size_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;

 protected:
  Container con;

 public:
  bool empty() const { return con.empty(); }
  size_type size() const { return con.size(); }
  reference front() { return con.front(); }
  const_reference front() const { return con.front(); }
  reference back() { return con.back(); }
  const_reference back() const { return con.back(); }
  void push(const value_type &value) { con.push_back(value); }
  void pop() { con.pop_front(); }

 private:
  friend bool operator==(const queue &lhs, const queue &rhs);
  friend bool operator<(const queue &lhs, const queue &rhs);
};

template<typename T, typename Container>
bool operator==(const queue<T, Container> &lhs,
				const queue<T, Container> &rhs) {
  return lhs.con == rhs.con;
}

template<typename T, typename Container>
bool operator<(const queue<T, Container> &lhs,
			   const queue<T, Container> &rhs) {
  return lhs.con < rhs.con;
}

} // namespace tinystl

#endif //TINYSTL__QUEUE_H_
