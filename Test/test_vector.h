//
// Created by polarnight on 24-8-11, 下午4:34.
//

#ifndef TINYSTL_TEST_TEST_VECTOR_H_
#define TINYSTL_TEST_TEST_VECTOR_H_

#include <iostream>
#include <vector>
#include "../vector.h"
#include "test.h"
#include <bits/stl_iterator.h>

template<typename T, typename Alloc1, typename Alloc2>
inline bool vec_equal(const std::vector<T, Alloc1> lhs,
					  const tinystl::vector<T, Alloc2> &rhs) {
  if (lhs.size() != rhs.size())
	return false;
  for (size_t i = 0; i != lhs.size(); ++i)
	if (lhs[i] != rhs[i])
	  return false;
  return true;
}

namespace tinystl {

void vector_test() {
  std::cout << "[============================================================"
			   "===]\n";
  std::cout << "[----------------- Run container test : vector "
			   "-----------------]\n";
  std::cout << "[-------------------------- API test "
			   "---------------------------]\n";
  int a[] = {1, 2, 3, 4, 5};
  tinystl::vector<int> v1;
  tinystl::vector<int> v2(10);
  tinystl::vector<int> v3(a, a + 5);
  tinystl::vector<int> v4(v3);
  tinystl::vector<int> v5 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  v1 = v2;
  PRINT(v1);
  PRINT(v2);
  PRINT(v3);
  PRINT(v4);
  PRINT(v5);
  FUN_AFTER(v1, v1.push_back(6));
  FUN_AFTER(v1, v1.insert(v1.end(), 1, 7));
  FUN_AFTER(v1, v1.insert(v1.begin() + 3, 2, 3));
  FUN_AFTER(v1, v1.pop_back());
  FUN_AFTER(v1, v1.erase(v1.begin()));
  FUN_AFTER(v1, v1.erase(v1.begin(), v1.begin() + 2));
  FUN_AFTER(v1, v1.swap(v4));
  FUN_VALUE(*v1.begin());
  FUN_VALUE(*v1.cbegin());
  FUN_VALUE(*(v1.end() - 1));
  FUN_VALUE(*(v1.cend() - 1));
  FUN_VALUE(*v1.rbegin());
  FUN_VALUE(*v1.crbegin());
  FUN_VALUE(*(v1.rend() - 1));
  FUN_VALUE(v1.front());
  FUN_VALUE(v1.back());
  FUN_VALUE(v1[0]);
  FUN_VALUE(v1.size());
  FUN_VALUE(v1.max_size());
  FUN_VALUE(v1.capacity());
  FUN_VALUE(v1.empty());
  FUN_AFTER(v1, v1.reserve(40));
  FUN_VALUE(v1.size());
  FUN_VALUE(v1.capacity());
  FUN_AFTER(v1, v1.resize(20, 5));
  FUN_AFTER(v1, v1.clear());
  FUN_VALUE(v1.size());
  std::cout << "[----------------------- end API test "
			   "---------------------------]\n";
}
} //namespace tinystl

#endif //TINYSTL_TEST_TEST_VECTOR_H_
