//
// Created by seventh on 2020/11/16.
//
#include <tuple>
#include "template_helper.h"

int main() {
    // remove_void
    static_assert(std::is_same_v<std::tuple<int, double>, remove_void_t<std::tuple<int, double, void>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int, double>, remove_void_t<std::tuple<int, void, double>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int, double>, remove_void_t<std::tuple<void, int, double>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int, double, float>,remove_void_t<std::tuple<int, double, void, void, float, void>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int, double, float>,remove_void_t<std::tuple<int, void, double, float>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int, double, float>,remove_void_t<std::tuple<void, int, double, float, void>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int, double, float>,remove_void_t<std::tuple<int, double, void, void, void, float, void, void>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int, double, float>,remove_void_t<std::tuple<void, void, void, int, void, double, void, float>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int, double, float>,remove_void_t<std::tuple<void, int, void, double, float, void, void, void>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int>, remove_void_t<std::tuple<int, void, void>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int>, remove_void_t<std::tuple<int, void, void, void, void, void, void>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int>, remove_void_t<std::tuple<void, void, int>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int>, remove_void_t<std::tuple<void, void, void, void, void, void, int>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int>, remove_void_t<std::tuple<int>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int, double>, remove_void_t<std::tuple<int, double>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<>, remove_void_t<std::tuple<void>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<>, remove_void_t<std::tuple<void, void>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<>, remove_void_t<std::tuple<void, void, void, void, void>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<>, remove_void_t<std::tuple<void, void, void, void, void>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<>, remove_void_t<std::tuple<>>>, "Test Fail");

    // concat
    static_assert(std::is_same_v<std::tuple<double, int, int, double, void>,concat_t<std::tuple<double, int>, std::tuple<int, double, void>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<double, int>,concat_t<std::tuple<double, int>, std::tuple<>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int, double, void>,concat_t<std::tuple<>, std::tuple<int, double, void>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<>,concat_t<std::tuple<>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<>,concat_t<std::tuple<>, std::tuple<>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<>,concat_t<std::tuple<>, std::tuple<>, std::tuple<>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int, double>,concat_t<std::tuple<>, std::tuple<int, double>, std::tuple<>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int, double>,concat_t<std::tuple<int>, std::tuple<>, std::tuple<double>>>, "Test Fail");
    static_assert(std::is_same_v<std::tuple<int, double, float>,concat_t<std::tuple<int>, std::tuple<double>, std::tuple<float>>>, "Test Fail");
    return 0;
}
