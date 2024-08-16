//
// Created by polarnight on 24-8-15, 下午5:32.
//

#ifndef TINYSTL__STACK_H_
#define TINYSTL__STACK_H_

/* <stack.h> 实现了 stack
 * 此处以 deque 为底部结构并封闭其头端开口，就能轻松实现
 * 但是同理，以 list 为底部结构并封闭其头端开口也可以，尚待实现 */

#include "deque.h"

namespace tinystl {

template<typename T, typename Container = tinystl::deque<T>>
class stack {
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
  reference top() { return con.back(); }
  const_reference top() const { return con.back(); }
  void push(const value_type &value) { con.push_back(value); }
  void pop() { con.pop_back(); }

 private:
  friend bool operator==(const stack &lhs, const stack &rhs);
  friend bool operator<(const stack &lhs, const stack &rhs);
};

template<typename T, typename Container>
bool operator==(const stack<T, Container> &lhs,
				const stack<T, Container> &rhs) {
  return lhs.con == rhs.con;
}

template<typename T, typename Container>
bool operator<(const stack<T, Container> &lhs, const stack<T, Container> &rhs) {
  return lhs.con < rhs.con;
}

}  // namespace tinystl

#endif //TINYSTL__STACK_H_
