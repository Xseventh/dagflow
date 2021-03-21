//
// Created by seventh on 2021/3/18.
//

#include "dag_flower_info.h"

int main() {
    dagflow::DagFlowerInfo<int, double> dag_info;
    dagflow::DagData<int, double> dag_data = dag_info.GetInputData();
    dagflow::DagData<int> dag_data_0 = dag_data.Flow<int>(
            [](const std::shared_ptr<int> &, const std::shared_ptr<double> &, std::shared_ptr<int> &) {});
    (dag_data.Get<0>() | dag_data_0).Flow<double>([](const std::shared_ptr<int> &, const std::shared_ptr<int> &, std::shared_ptr<double> &) {});

}
