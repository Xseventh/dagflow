//
// Created by seventh on 2021/2/20.
//
#include <iostream>
#include <string>
#include "node.h"
#include "data_manager.h"

void func(const std::shared_ptr<int> &a,
          const std::shared_ptr<double> &b,
          std::shared_ptr<double> &,
          std::shared_ptr<int> &,
          std::shared_ptr<std::string> &) {
    std::cout << *a << "," << *b;
}

int main() {
    DataManager dataManager;
    dagflow::Node<std::tuple<int, double>, std::tuple<double, int, std::string>> node(func, dataManager);
    std::shared_ptr<int> a = std::make_shared<int>(10);
    std::shared_ptr<double> b = std::make_shared<double>(20);
    node.Submit(a, b);
}