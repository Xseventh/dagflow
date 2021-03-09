//
// Created by seventh on 2021/3/7.
//
#include <iostream>
#include <chrono>
#include "dagflow/dag_runner.h"

std::mutex print_lock;

void funcA(const std::shared_ptr<int> &data,
           std::shared_ptr<int> &output) {
    std::lock_guard lock(print_lock);
    std::cout << "FuncA : " << *data << " thread id : " << std::this_thread::get_id() << std::endl;
    output = std::make_shared<int>(*data * 2);
}

void funcB(const std::shared_ptr<int> &data,
           std::shared_ptr<double> &output) {
    std::lock_guard lock(print_lock);
    std::cout << "FuncB : " << *data << " thread id : " << std::this_thread::get_id() << std::endl;
    output = std::make_shared<double>(*data * 2 + 0.5);
}

void funcC(const std::shared_ptr<int> &data1,
           const std::shared_ptr<double> &data2) {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::lock_guard lock(print_lock);
    std::cout << "FuncC data1 : " << *data1 << " data2 : " << *data2 << " thread id : " << std::this_thread::get_id()
              << std::endl;
}

int main() {
    dagflow::DagRunner<int, double, std::string> dagRunner;

    dagflow::DagData<int, double, std::string> input_data = dagRunner.m_source_data;
    dagflow::DagData<int> data_int = input_data.Get<0>();
    dagflow::DagData<int> funA_result = data_int.operator>>= <int>(funcA);
    dagflow::DagData<double> funB_result = funA_result.operator>>= <double>(funcB);
    (funA_result | funB_result).operator>>=(funcC);
    (funA_result | funB_result).operator>>=(funcC);
    dagRunner.Submit(std::make_unique<int>(1), std::make_unique<double>(2), std::make_unique<std::string>("3"));
    return 0;
}