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

void foodddd(const std::shared_ptr<double> &d1,
            const std::shared_ptr<double> &d2,
            const std::shared_ptr<double> &d3,
            const std::shared_ptr<double> &d4,
            std::shared_ptr<int> &i4) {
    std::lock_guard _(print_mutex);
    std::cout << "fooddd thread_id : " << std::this_thread::get_id() << std::endl;
    i4 = std::make_shared<int>(*d1 + *d2 + *d3 + *d4);
}

class Void{};

void fooprintd(const std::shared_ptr<double> &d1) {
    std::lock_guard _(print_mutex);
    std::cout << "double : " << *d1 << std::endl;
}

void fooprinti(const std::shared_ptr<int> &d1) {
    std::lock_guard _(print_mutex);
    std::cout << "int : " << *d1 << std::endl;
}

int main() {
    dagflow::DagFlowerInfo<int, double> dag_info;
    dagflow::DagData<int, double> input_data = dag_info.GetInputData();
    dagflow::DagData<double> d1 = input_data.Get<0>().Flow<double>(fooi2d).Flow<double>(food2d);
    dagflow::DagData<double> d2 = input_data.Get<0>().Flow<double>(fooi2d).Flow<double>(food2d).Flow<double>(food2d);
    dagflow::DagData<double> d3 = input_data.Get<0>().Flow<double>(fooi2d).Flow<double>(food2d).Flow<double>(food2d).Flow<double>(food2d);
    dagflow::DagData<int> i1 = (d1 | d2 | d3 | input_data.Get<1>()).Flow<int>(foodddd);
    i1.Flow<>(fooprinti);
    d1.Flow<>(fooprintd);
    d2.Flow<>(fooprintd);
    d3.Flow<>(fooprintd);

    dagflow::DagFlower<int, double> dagflower(std::move(dag_info));
    dagflower.Submit(new int(2), new double(10));

    dagflower.Submit(std::make_unique<int>(1), new double(10));

}