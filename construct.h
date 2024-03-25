//
// Created by polarnight on 24-3-25, 下午9:15.
//

#ifndef TINYSTL_CONSTRUCT_H
#define TINYSTL_CONSTRUCT_H

/* 这个头文件包含两个函数 construct() 和 destroy()
 * construct：负责对象的构造
 * destroy：负责对象的析构 */

#include <new>

namespace mystl {
    /* construct 构造对象 */

    template<typename Ty>
    void construct(Ty *ptr) {
        new(ptr) Ty();
    }

    template<typename Ty1, typename Ty2>
    void construct(Ty1 *ptr, const Ty2 &value) {
        new(ptr) Ty1(value);
    }

    template<typename Ty, typename ...Args>
    void construct(Ty *ptr, Args &&...args) {
        new(ptr) Ty(mystl::forward<Args>(args)...);
    }

    /* destroy 析构对象 */

    /* destroy 版本一 接受一个指针 */
    /* 特化版本 处理平凡析构的情况 不调用析构操作 */
    template<typename Ty>
    void destroy_one(Ty *, std::true_type) {}

    /* 通用版本 处理非平凡析构的情况 */
    template<typename Ty>
    void destroy_one(Ty *ptr, std::false_type) {
        if (ptr) ptr->~Ty();
    }

    template<typename Ty>
    void destroy(Ty *ptr) {
        destroy_one(ptr, std::is_trivially_destructible<Ty>{});
    }

    /* destroy 版本二 接受两个迭代器 */
    /* 特化版本 处理平凡析构的情况 不调用析构操作 */
    template<typename ForwardIter>
    void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

    /* 通用版本 处理非平凡析构的情况 */
    template<typename ForwardIter>
    void destroy_cat(ForwardIter first, ForwardIter last, std::false_type) {
        for (; first < last; ++first)
            destroy(&*first);
    }

    template<typename ForwardIter>
    void destroy(ForwardIter first, ForwardIter last) {
        destroy_cat(first, last, std::is_trivially_destructible<typename iteractor_traits<ForwardIter>::value_type>{})
    }

}

#endif //TINYSTL_CONSTRUCT_H
