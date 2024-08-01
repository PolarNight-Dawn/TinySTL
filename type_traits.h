//
// Created by polarnight on 24-7-28, 下午6:38.
//

#ifndef TINYSTL__TYPE_TRAITS_H_
#define TINYSTL__TYPE_TRAITS_H_

/* <type_traits.h> 萃取型别的类型 */

namespace tinystl {
struct _true_type { };
struct _false_type { };

template<typename type>
struct type_traits {
  using has_trivial_default_constructor   = _false_type;
  using has_trivial_copy_constructtor     = _false_type;
  using has_trivial_assignment_operator   = _false_type;
  using has_trivial_destructor            = _false_type;
  using is_POD_type                       = _false_type;
};

// 针对各种算术整型的特化版本
template<>
struct type_traits<bool> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

template<>
struct type_traits<char> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

template<>
struct type_traits<unsigned char> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

template<>
struct type_traits<signed char> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

template<>
struct type_traits<wchar_t> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

template<>
struct type_traits<short> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

template<>
struct type_traits<unsigned short> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

template<>
struct type_traits<int> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

template<>
struct type_traits<unsigned int> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

template<>
struct type_traits<long> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

template<>
struct type_traits<unsigned long> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

template<>
struct type_traits<long long> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

template<>
struct type_traits<unsigned long long> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

template<>
struct type_traits<float> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

template<>
struct type_traits<double> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

template<>
struct type_traits<long double> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

// 针对指针的偏特化版本
template<typename type>
struct type_traits<type*> {
  using has_trivial_default_constructor   = _true_type;
  using has_trivial_copy_constructtor     = _true_type;
  using has_trivial_assignment_operator   = _true_type;
  using has_trivial_destructor            = _true_type;
  using is_POD_type                       = _true_type;
};

}

#endif //TINYSTL__TYPE_TRAITS_H_
