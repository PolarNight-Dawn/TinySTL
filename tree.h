//
// Created by polarnight on 24-8-7, 下午5:39.
//

#ifndef TINYSTL__TREE_H_
#define TINYSTL__TREE_H_

/* <tree.h> 实现了 RB-tree（红黑树） */

#include "memory.h"

namespace tinystl {
using rb_tree_color_type = bool;
const rb_tree_color_type rb_tree_red = false;
const rb_tree_color_type rb_tree_black = true;

struct rb_tree_node_base {
  using color_type = rb_tree_color_type;
  using base_ptr = rb_tree_node_base *;

  color_type color;
  base_ptr parent;
  base_ptr left;
  base_ptr right;

  static base_ptr mininum(base_ptr x) {
	while (x->left != nullptr) x = x->left;
	return x;
  }

  static base_ptr maxinum(base_ptr x) {
	while (x->right != nullptr) x= x->right;
	return x;
  }
}; // tree node base end

template<typename Value>
struct rb_tree_node : public rb_tree_node_base {
  using link_type = rb_tree_node<Value>*;
  Value value_filed;
}; // tree node end

}

#endif //TINYSTL__TREE_H_
