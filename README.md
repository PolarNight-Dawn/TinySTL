# TinySTL

## 介绍

项目有一定的注释，但主要是对框架的一些解释。如果你想深入理解具体的实现原理，那么《STL 源码剖析》你值得拥有😉，
可能与项目的实现有所变化，但也只是些细枝末节罢了。项目中的思路过程整理成了几篇博文，如果你想要了解一下的话，
[点击这里](https://polarnight-dawn.cn/2024/07/30/STL-%E5%89%96%E6%9E%90%EF%BC%9A%E7%A9%BA%E9%97%B4%E9%85%8D%E7%BD%AE%E5%99%A8%EF%BC%88allocator%EF%BC%89/)进入

请注意，本项目在还不定期的更新与优化中，很多模块还并未实现，至于性能的进一步优化更是鞭长莫及， 我尽可能地保证已实现部分的正确性😇

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
      - stack（stack.h）
      - queue（queue.h）
    - heap（heap.h）
  - 关联式容器 associative containers
    - RB-tree（tree.h） 
      - set/multiset（set.h）
      - map/multimap（map.h）
    - hashtable（hashtable.h）

## 参考资料
- 《STL 源码剖析》by 候捷
- GNU ISO C++ Library