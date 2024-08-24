//
// Created by polarnight on 24-8-11, 下午6:57.
//

#include "test_vector.h"
#include "test_list.h"
#include "test_deque.h"
#include "test_tree.h"

int main() {

  tinystl::vector_test();
  tinystl::list_test();
  tinystl::deque_test();
  tinystl::tree_test();

  return 0;
}