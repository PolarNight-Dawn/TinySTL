# TinySTL

想要深入理解 STL 总得造一个玩具轮子来玩玩吧

## 目录
- 空间配置器 allocator
  - SGI 标准的空间配置器，std::allocator（allocator.h）
  - 构造与析构工具（construct.h）
    - construct() 
    - destroy()
  - 空间的配置与释放 std::alloc（alloc.h）
    - 第一级配置器 malloc_alloc
    - 第二级配置器 default_alloc
  - 内存基本处理工具（uninitialized.h）
    - uninitialized_copy()
    - uninitialized_fill()
    - uninitialized_fill_n()
- 迭代器 iterators
  - iterator_traits（iterator.h）
  - __type_traits（type_traits.h）
- 容器 containers
  - 序列式容器 sequence containers
    - vector（vector.h）
    - list（list.h）
    - deque（deque.h）
    - heap（heap.h）

## 参考资料
- 《STL源码剖析》by 候捷
- GNU ISO C++ Library