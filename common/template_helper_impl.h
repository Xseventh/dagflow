//
// Created by seventh on 2020/11/16.
//

#ifndef AIOLOS_EXPERIMENT_PROJECT_EXPR_007_COMMON_TEMPLATE_HELPER_IMPL_H_
#define AIOLOS_EXPERIMENT_PROJECT_EXPR_007_COMMON_TEMPLATE_HELPER_IMPL_H_
#include "template_helper.h"
template<template<typename ...> class C, typename... T0, typename... T1, typename... Args>
struct concat<C<T0...>, C<T1...>, Args...> {
    using type = std::conditional_t<(sizeof...(Args) > 0), concat_t<C<T0..., T1...>, Args...>, C<T0..., T1...>>;
};

template<template<typename ...> class C, typename... Args>
struct concat<C<Args...>> {
    using type = C<Args...>;
};

template<template<typename...> class C, typename T, typename... Args>
struct remove_void<C<T, Args...>> {
    using type = std::conditional_t<std::is_same_v<T, void>,
                                    remove_void_t<C<Args...>>,
                                    concat_t<C<T>, remove_void_t<C<Args...>>>>;
};

template<template<typename...> class C>
struct remove_void<C<>> {
    using type = C<>;
};

template<typename T>
struct tag {
    using type = T;
};
#endif //AIOLOS_EXPERIMENT_PROJECT_EXPR_007_COMMON_TEMPLATE_HELPER_IMPL_H_
