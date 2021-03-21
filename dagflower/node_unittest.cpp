//
// Created by seventh on 2021/2/20.
//
#include <iostream>
#include <string>

#include "dagflow/dagflower/executor/node/node.h"

void func(const std::shared_ptr<const int> &a,
          const std::shared_ptr<const double> &b,
          std::shared_ptr<double> &c,
          std::shared_ptr<int> &d,
          std::shared_ptr<std::string> &e) {
    std::cout << *a << "," << *b << std::endl;
    c = std::make_shared<double>(*a * *b);
    d = std::make_shared<int>(*a + *b);
    e = std::make_shared<std::string>("hello world!");
}

int main() {
    dagflow::common::ThreadPool thread_pool(1);
    std::shared_ptr<std::shared_ptr<const int>> a = std::make_shared<std::shared_ptr<const int>>(
            std::make_shared<const int>(10));
    std::shared_ptr<std::shared_ptr<const double>> b = std::make_shared<std::shared_ptr<const double>>(
            std::make_shared<const double>(20));
    std::shared_ptr<std::shared_ptr<double>> c = std::make_shared<std::shared_ptr<double>>();
    std::shared_ptr<std::shared_ptr<int>> d = std::make_shared<std::shared_ptr<int>>();
    std::shared_ptr<std::shared_ptr<std::string>> e = std::make_shared<std::shared_ptr<std::string>>();

    dagflow::detail::Node<std::tuple<const int, const double>, std::tuple<double, int, std::string>> node(func, a, b, c, d, e, 0, thread_pool);
    node.Run();
    std::cout << "result c = " << **c << std::endl
              << "result d = " << **d << std::endl
              << "result e = " << **e << std::endl;
    return 0;
}