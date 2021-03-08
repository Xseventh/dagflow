//
// Created by seventh on 2020/11/16.
//

#ifndef DAGFLOW_COMMON_TEMPLATE_HELPER_H_
#define DAGFLOW_COMMON_TEMPLATE_HELPER_H_

#include <type_traits>

namespace dagflow::common {

template<typename... Args>
struct concat;

template<typename... Args>
using concat_t = typename concat<Args...>::type;

template<typename T>
struct remove_void;

template<typename T>
using remove_void_t = typename remove_void<T>::type;

template<typename T>
struct tag;

template<typename T>
using block_deduction_t = typename tag<T>::type;

}

#include "template_helper_impl.h"

#endif //DAGFLOW_COMMON_TEMPLATE_HELPER_H_
