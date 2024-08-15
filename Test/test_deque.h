//
// Created by polarnight on 24-8-14, 下午10:33.
//

#ifndef TINYSTL_TEST_TEST_DEQUE_H_
#define TINYSTL_TEST_TEST_DEQUE_H_

#include <iostream>
#include "test.h"
#include "../deque.h"
#include <deque>
#include <bits/stl_deque.h>

namespace tinystl{

void deque_test() {
  std::cout << "[============================================================"
			   "===]\n";
  std::cout << "[----------------- Run container test : deque "
			   "-------------------]\n";
  std::cout << "[-------------------------- API test "
			   "---------------------------]\n";
  int a[] = {1, 2, 3, 4, 5};
  tinystl::deque<int> l1;
  tinystl::deque<int> l2(10);
  tinystl::deque<int> l3(10, 1);
  tinystl::deque<int> l4(a, a + 5);
  tinystl::deque<int> l5(l4);
  tinystl::deque<int> l6 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  tinystl::deque<int> l7(l6.begin(), l6.end());
  l1 = l3;
  PRINT(l1);
  PRINT(l2);
  PRINT(l3);
  PRINT(l4);
  PRINT(l5);
  PRINT(l6);
  PRINT(l7);
  FUN_AFTER(l1, l1.push_back(6));
  FUN_AFTER(l1, l1.push_front(8));
  FUN_AFTER(l1, l1.insert(l1.end(), 7));
  FUN_AFTER(l1, l1.insert(l1.begin(), 2, 3));
  FUN_AFTER(l1, l1.pop_back());
  FUN_AFTER(l1, l1.pop_front());
  FUN_AFTER(l1, l1.erase(l1.begin()));
  FUN_AFTER(l1, l1.erase(l1.begin(),l1.begin() + 2));
  FUN_AFTER(l1, l1.swap(l4));
  FUN_VALUE(*l1.begin());
  FUN_VALUE(*l1.cbegin());
  FUN_VALUE(*(--l1.end()));
  FUN_VALUE(*(--l1.cend()));
  FUN_VALUE(*l1.rbegin());
  FUN_VALUE(*(--l1.rend()));
  FUN_VALUE(l1.front());
  FUN_VALUE(l1.back());
  FUN_VALUE(l1.size());
  FUN_VALUE(l1.max_size());
  FUN_VALUE(l1.empty());
  FUN_VALUE(l1.size());
  FUN_AFTER(l1, l1.resize(30, 5));
  FUN_AFTER(l1, l1.clear());
  FUN_VALUE(l1.size());
  std::cout << "[----------------------- end API test "
			   "---------------------------]\n";
}

} // namespace tinystl

#endif //TINYSTL_TEST_TEST_DEQUE_H_
