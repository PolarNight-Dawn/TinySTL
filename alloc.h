//
// Created by polarnight on 24-3-29, 上午1:07.
//

#ifndef TINYSTL_ALLOC_H
#define TINYSTL_ALLOC_H

#include <iostream>

#include "construct.h"

/* 具有 SGI 特色的两级分配器 */

namespace TinySTL {
    /* 使用 malloc 和 free 实现的一级分配器, 设置 OOM 时的 new_handler */
    class malloc_alloc_template {
    public:
        static void *allocate(size_t);

        static void deallocate(void *ptr);

        static void *reallocate(void *, size_t, size_t new_sz);

        static void (*set_malloc_handler(void(*f)()))();

    private:
        static void *oom_malloc(size_t);

        static void *oom_realloc(void *, size_t);

        static void (*malloc_alloc_oom_handler)();
    };

    void *malloc_alloc_template::allocate(size_t n) {
        void *result = malloc(n);
        if (result == nullptr) result = oom_malloc(n);
        return result;
    }

    void malloc_alloc_template::deallocate(void *ptr) {
        free(ptr);
    }

    void *malloc_alloc_template::reallocate(void *ptr, size_t, size_t new_sz) {
        void *result = realloc(ptr, new_sz);
        if (result == nullptr) oom_realloc(ptr, new_sz);
        return result;
    }

    void (*malloc_alloc_template::malloc_alloc_oom_handler)() = nullptr;

    void (* malloc_alloc_template::set_malloc_handler(void (*f)()))() {
        void (* old)() = malloc_alloc_oom_handler;
        malloc_alloc_oom_handler = f;
        return old;
    }

    void* malloc_alloc_template::oom_malloc(size_t n) {
        if (malloc_alloc_oom_handler == nullptr) {
            std::cerr << "out of memory" << std::endl;
            exit(1);
        }
        void* result;
        for (;;) {
            malloc_alloc_oom_handler();
            result = malloc(n);
            if (result)
                return result;
        }
    }

    void* malloc_alloc_template::oom_realloc(void* ptr, size_t new_sz) {
        if (malloc_alloc_oom_handler == nullptr) {
            std::cerr << "out of memory" << std::endl;
            exit(1);
        }
        void* result;
        for (;;) {
            malloc_alloc_oom_handler();
            result = realloc(ptr, new_sz);
            if (result)
                return result;
        }
    }
}

#endif //TINYSTL_ALLOC_H
