# TinySTL

想要深入理解 STL 总得造一个玩具轮子来玩玩吧

## 目录
- 空间配置器 allocator
  - SGI 标准的空间配置器，std::allocator（allocator.h）
  - 构造与析构工具，construct() 和 destroy()（construct.h）
  - 空间的配置与释放 std::alloc (alloc.h)
    - 第一级配置器 malloc_alloc
    - 第二级配置器 default_alloc

## 参考资料
- 《STL源码剖析》by 候捷