//
// Created by seventh on 2021/3/12.
//
#include <iostream>
#include "dagflow/dagflower/dag_flower.h"

std::mutex print_mutex;

void fooi2d(const std::shared_ptr<int> &d1,
            std::shared_ptr<double> &d2) {
    std::lock_guard _(print_mutex);
    std::cout << "fooi2d thread_id : " << std::this_thread::get_id() << std::endl;
    d2 = std::make_shared<double>(*d1 * 2);
}

void food2d(const std::shared_ptr<double> &d1,
            std::shared_ptr<double> &d2) {
    std::lock_guard _(print_mutex);
    std::cout << "food2d thread_id : " << std::this_thread::get_id() << std::endl;
    d2 = std::make_shared<double>(*d1 * 2);
}

void fooddd(const std::shared_ptr<double> &d1,
            const std::shared_ptr<double> &d2,
            const std::shared_ptr<double> &d3,
            std::shared_ptr<int> &d4) {
    std::lock_guard _(print_mutex);
    std::cout << "fooddd thread_id : " << std::this_thread::get_id() << std::endl;
    d4 = std::make_shared<int>(*d1 + *d2 + *d3);
}

class Void{};

void fooprintd(const std::shared_ptr<double> &d1,
               std::shared_ptr<Void> &) {
    std::lock_guard _(print_mutex);
    std::cout << "double : " << *d1 << std::endl;
}

void fooprinti(const std::shared_ptr<int> &d1,
               std::shared_ptr<Void> &) {
    std::lock_guard _(print_mutex);
    std::cout << "int : " << *d1 << std::endl;
}

int main() {
    dagflow::DagFlowerInfo<int> dag_info;
    auto input_data = dag_info.GetInputData();
    auto d1 = input_data.Flow<double>(fooi2d).Flow<double>(food2d);
    auto d2 = input_data.Flow<double>(fooi2d).Flow<double>(food2d).Flow<double>(food2d);
    auto d3 = input_data.Flow<double>(fooi2d).Flow<double>(food2d).Flow<double>(food2d).Flow<double>(food2d);
    auto i1 = (d1 | d2 | d3).Flow<int>(fooddd);
    i1.Flow<Void>(fooprinti);
    d1.Flow<Void>(fooprintd);
    d2.Flow<Void>(fooprintd);
    d3.Flow<Void>(fooprintd);

    dagflow::DagFlower<int> dagflower(std::move(dag_info));
    dagflower.Submit(new int(2));

    dagflower.Submit(std::make_unique<int>(1));

}