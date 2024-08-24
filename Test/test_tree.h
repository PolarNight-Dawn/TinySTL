//
// Created by polarnight on 24-8-23, 下午6:44.
//

#ifndef TINYSTL_TEST_TEST_TREE_H_
#define TINYSTL_TEST_TEST_TREE_H_

#include <iostream>
#include "test.h"
#include "../tree.h"
#include <bits/stl_tree.h>

namespace tinystl {

void tree_test() {
  std::cout << "[============================================================"
			   "===]\n";
  std::cout << "[----------------- Run container test : RB-tree "
			   "-------------------]\n";
  std::cout << "[-------------------------- API test "
			   "---------------------------]\n";
  int a[] = {1, 2, 3, 4, 5};
  tinystl::rb_tree<int, int, std::_Identity<int>> l1;
  tinystl::rb_tree<int, int, std::_Identity<int>> l2;
  l1 = l2;
  PRINT(l1);
  PRINT(l2);
  FUN_AFTER(l1, l1.insert_equal(7));
  FUN_AFTER(l1, l1.insert_equal(l1.begin(), 7));
  FUN_AFTER(l1, l1.erase(l1.begin()));
/*  FUN_AFTER(l1, l1.erase(l1.begin(), l1.begin() + 2));*/
  FUN_VALUE(*l1.begin());
  FUN_VALUE(*l1.cbegin());
  FUN_VALUE(*(--l1.end()));
  FUN_VALUE(*(--l1.cend()));
  FUN_VALUE(*l1.rbegin());
  FUN_VALUE(*(--l1.rend()));
  FUN_VALUE(l1.size());
  FUN_VALUE(l1.max_size());
  FUN_VALUE(l1.empty());
  FUN_AFTER(l1, l1.clear());
  std::cout << "[----------------------- end API test "
			   "---------------------------]\n";
}

} // namespace tinystl

#endif //TINYSTL_TEST_TEST_TREE_H_
