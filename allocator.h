//
// Created by polarnight on 24-3-26, 下午10:02.
//

#ifndef TINYSTL_ALLOCATOR_H
#define TINYSTL_ALLOCATOR_H

/* 这个头文件包含一个模板类 allocator， 仅仅对于 ::operator new 和 ::operator delete 的简单封装 */

#include "construct.h"

namespace TinySTL {
    template<typename T>
    class allocator {
    public:
        /* STL 要求的类型别名定义 */
        typedef T value_type; // using value_type = T;
        typedef T *pointer;
        typedef const T *const_pointer;
        typedef T &reference;
        typedef const T &const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    public:
        /* STL 要求的类接口，使用静态函数实现使频繁调用下的开销减少 */
        /* 负责分配内存 */
        static T *allocate();

        static T *allocate(size_type n);

        /* 负责释放内存 */
        static void deallocate(T *ptr);

        static void deallocate(T *ptr, size_type n);

        /* 负责构造对象 */
        static void construct(T *ptr);

        static void construct(T *ptr, const T &value);

        static void construct(T *ptr, T &&value);

        template<class... Args>
        static void construct(T *ptr, Args &&...args);

        /* 负责析构对象 */
        static void destroy(T *ptr);

        static void destroy(T *first, T *last);
    };

    template<typename T>
    T *allocator<T>::allocate() {
        return static_cast<T *>(::operator new(sizeof(T)));
    }

    template<typename T>
    T *allocator<T>::allocate(size_type n) {
        if (n == 0) return nullptr;
        return static_cast<T *>(::operator new(n * sizeof(T)));
    }

    template<typename T>
    void allocator<T>::deallocate(T *ptr) {
        if (!ptr) return;
        ::operator delete(ptr);
    }

    template<typename T>
    void allocator<T>::deallocate(T *ptr, size_type) {
        if (!ptr) return;
        ::operator delete(ptr);
    }

    template<typename T>
    void allocator<T>::construct(T *ptr) {
        TinySTL::construct(ptr);
    }

    template<typename T>
    void allocator<T>::construct(T *ptr, const T &value) {
        TinySTL::construct(ptr, value);
    }

    template<typename T>
    void allocator<T>::construct(T *ptr, T &&value) {
        TinySTL::construct(ptr, TinySTL::move(value));
    }

    template<typename T>
    template<typename ...Args>
    void allocator<T>::construct(T *ptr, Args &&...args) {
        TinySTL::construct(ptr, TinySTL::forward<Args>(args)...);
    }

    template<typename T>
    void allocator<T>::destroy(T *ptr) {
        TinySTL::destroy(ptr);
    }

    template<typename T>
    void allocator<T>::destroy(T *first, T *last) {
        TinySTL::destroy(first, last);
    }
}

#endif //TINYSTL_ALLOCATOR_H
