//
// Created by polarnight on 24-8-7, 上午11:03.
//

#ifndef TINYSTL__HEAP_H_
#define TINYSTL__HEAP_H_

/* <heap.h> 定义了一组 max-heap 算法，用以实现 binary max heap，作为 priority queue 的底层机制 */

#include "iterator.h"

namespace tinystl {
/* push_heap 接受两个迭代器，表示 heap 底部容器（vector）的首尾，并且新元素已插入到底部容器的尾端 */
template<typename RandomAccessIterator, typename Distance, typename T>
void push_heap(RandomAccessIterator first, Distance hole_index, Distance top_index, T value) {
  Distance parent = (hole_index - 1) / 2;
  while (hole_index > top_index && *(first + parent) < value) {
	*(first + hole_index) = *(first + parent);
	hole_index = parent; // percolate up
	parent = (hole_index - 1) / 2;
  }
  *(first + hole_index) = value;
}

template<typename RandomAccessIterator>
inline void push_heap_aux(RandomAccessIterator first, RandomAccessIterator last) {
  using distance_type = typename iterator_traits<RandomAccessIterator>::difference_type;
  using value_type = typename iterator_traits<RandomAccessIterator>::value_type;

  push_heap(first,
			static_cast<distance_type>((last - first) - 1),
			static_cast<distance_type>(0),
			static_cast<value_type>(*(last - 1)));
}

template<typename RandomAccessIterator, typename Distance, typename T>
void adjust_heap(RandomAccessIterator first, Distance hole_index, Distance len, T value) {
  Distance top_index = hole_index;
  Distance second_child = 2 * (hole_index + 1);

  while (second_child < len) {
	if (*(first + second_child) < *(first + second_child - 1))
	  second_child--;
	*(first + hole_index) = *(first + second_child); // percolate down
	hole_index = second_child;
	second_child = 2 * (second_child + 1);
  }

  if (second_child == len) {
	*(first + hole_index) = *(first + second_child); // percolate down
	hole_index = second_child - 1;
  }

  push_heap(first, hole_index, top_index, value);
}

template<typename RandomAccessIterator, typename Distance, typename T>
inline void pop_heap_aux(RandomAccessIterator first,
						 RandomAccessIterator last,
						 RandomAccessIterator result,
						 T value,
						 Distance *) {
  *result = *first;
  adjust_heap(first, Distance(0), Distance(last - first), value);
}

/* push_heap 接受两个迭代器，表示 heap 底部容器（vector）的首尾 */
template<typename RandomAccessIterator>
inline void ppo_heap(RandomAccessIterator first, RandomAccessIterator last) {
  using distance_type = typename iterator_traits<RandomAccessIterator>::difference_type;
  using value_type = typename iterator_traits<RandomAccessIterator>::value_type;

  pop_heap_aux(first,
			   last - 1,
			   last - 1,
			   static_cast<value_type>(*(last - 1)),
			   static_cast<distance_type>(first));
}

/* sort_heap
 * 获取一个递增序列
 * 接受两个迭代器，表示 heap 底部容器（vector）的首尾 */
template<typename RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
  while (last - first > 1)
	ppo_heap(first, last--);
}

/* make_heap
 * 将 [first, last) 排列为一个 heap
 * 接受两个迭代器，表示 heap 底部容器（vector）的首尾 */
template<typename RandomAccessIterator>
void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
  using distance_type = typename iterator_traits<RandomAccessIterator>::difference_type;
  using value_type = typename iterator_traits<RandomAccessIterator>::value_type;

  if (last - first < 2) return;
  distance_type len = last - first;
  distance_type hole_index = (len - 2) / 2;

  while (true) {
	adjust_heap(first, hole_index, len, static_cast<value_type>(*(first + hole_index)));
	if (hole_index == 0) return;
	--hole_index;
  };
}

} // namespace tinystl

#endif //TINYSTL__HEAP_H_
