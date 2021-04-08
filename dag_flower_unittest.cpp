//
// Created by seventh on 2021/3/12.
//
#include <iostream>
#include <cassert>
#include "dagflow/dagflower/dag_flower.h"

void VectorSort(const std::shared_ptr<std::vector<int>> &p_vector,
                std::shared_ptr<std::vector<int>> &p_sorted_vector_result) {
    std::sort(p_vector->begin(), p_vector->end());
    p_sorted_vector_result = p_vector;
}

void VectorMultiply(const std::shared_ptr<std::vector<int>> &p_vector0,
                    const std::shared_ptr<std::vector<int>> &p_vector1,
                    std::shared_ptr<std::vector<int>> &p_vector_result) {
    std::vector<int> vector_result(p_vector0->size() + p_vector1->size() - 1);
    for (size_t i = 0; i < p_vector0->size(); i++) {
        for (size_t j = 0; j < p_vector1->size(); j++) {
            vector_result[i + j] += (*p_vector0)[i] * (*p_vector1)[j];
        }
    }
    p_vector_result = std::make_shared<std::vector<int>>(std::move(vector_result));
}

void VectorAdd(const std::shared_ptr<std::vector<int>> &p_vector0,
               const std::shared_ptr<std::vector<int>> &p_vector1,
               std::shared_ptr<std::vector<int>> &p_vector_result) {
    assert(p_vector0->size() == p_vector1->size()); // 目前框架还没有能力处理exception，需要调用者保证这里长度相等
    std::vector<int> vector_result(p_vector0->size());
    for (size_t i = 0; i < vector_result.size(); i++) {
        vector_result[i] = (*p_vector0)[i] + (*p_vector1)[i];
    }
    p_vector_result = std::make_shared<std::vector<int>>(std::move(vector_result));
}

std::mutex print_mutex;

void VectorPrint(const std::shared_ptr<std::vector<int>> &p_vector) {
    std::lock_guard<std::mutex> lock(print_mutex);
    for (auto data : *p_vector) {
        std::cout << data << " ";
    }
    std::cout << std::endl;
}

int main() {
    /*
     * Sample流程 输入数据为3个长度相等的vector<int>数据
     * 需要将三个vector分别进行排序
     * 然后将三个vector的结果进行两两卷积
     * 最后将三个vector两两卷积后的结果相加
     */
    dagflow::DagFlowerInfo<std::vector<int>, std::vector<int>, std::vector<int>> dag_info; // 定义dagInfo,Dag图的信息都在Info里
    // 以下建立Info过程
    dagflow::DagData<std::vector<int>, std::vector<int>, std::vector<int>> input_data = dag_info.GetInputData();

    // 将三个vector分别进行排序
    std::vector<dagflow::DagData<std::vector<int>>> sorted_vector_data;
    sorted_vector_data.emplace_back(input_data.Get<0>().Flow<std::vector<int>>(VectorSort));
    sorted_vector_data.emplace_back(input_data.Get<1>().Flow<std::vector<int>>(VectorSort));
    sorted_vector_data.emplace_back(input_data.Get<2>().Flow<std::vector<int>>(VectorSort));

    // 将三个vector的结果进行两两卷积
    std::vector<dagflow::DagData<std::vector<int>>> multiply_result_vector_data;
    for (const auto &data_i : sorted_vector_data) {
        for (const auto &data_j : sorted_vector_data) {
            multiply_result_vector_data.emplace_back((data_i | data_j).Flow<std::vector<int>>(VectorMultiply));
        }
    }

    // 将三个vector两两卷积后的结果相加
    auto result_vector_data = std::make_unique<dagflow::DagData<std::vector<int>>>(multiply_result_vector_data[0]);
    for (size_t i = 1; i < multiply_result_vector_data.size(); i++) {
        result_vector_data = std::make_unique<dagflow::DagData<std::vector<int>>>(
                (*result_vector_data | multiply_result_vector_data[i]).Flow<std::vector<int>>(VectorAdd));
    }

    // Print显示三个vector的结果
    result_vector_data->Flow<>(VectorPrint);
    //dag_info 建立完成

    // 通过 info 构建出dag_flower
    dagflow::DagFlower<std::vector<int>, std::vector<int>, std::vector<int>> dag_flower(std::move(dag_info)); // 目前还没有写info的拷贝构造，所以只能先用move

    //以下是往dag_flower中Submit数据，提交6次数据，这6次数据在执行过程中互不干扰
    {
        auto input_data0 = std::make_unique<std::vector<int>>(std::vector{1, 2, 3, 4});
        auto input_data1 = std::make_unique<std::vector<int>>(std::vector{1, 2, 3, 4});
        auto input_data2 = std::make_unique<std::vector<int>>(std::vector{1, 2, 3, 4});
        dag_flower.Submit(std::move(input_data0), std::move(input_data1), std::move(input_data2));
    }

    {
        auto input_data0 = std::make_unique<std::vector<int>>(std::vector{1, 1, 1, 1});
        auto input_data1 = std::make_unique<std::vector<int>>(std::vector{1, 1, 1, 1});
        auto input_data2 = std::make_unique<std::vector<int>>(std::vector{1, 1, 1, 1});
        dag_flower.Submit(std::move(input_data0), std::move(input_data1), std::move(input_data2));
    }

    {
        auto input_data0 = std::make_unique<std::vector<int>>(std::vector{1, 2, 3, 4});
        auto input_data1 = std::make_unique<std::vector<int>>(std::vector{1, 2, 3, 4});
        auto input_data2 = std::make_unique<std::vector<int>>(std::vector{1, 2, 3, 4});
        dag_flower.Submit(std::move(input_data0), std::move(input_data1), std::move(input_data2));
    }

    {
        auto input_data0 = std::make_unique<std::vector<int>>(std::vector{1, 1, 1, 1});
        auto input_data1 = std::make_unique<std::vector<int>>(std::vector{1, 1, 1, 1});
        auto input_data2 = std::make_unique<std::vector<int>>(std::vector{1, 1, 1, 1});
        dag_flower.Submit(std::move(input_data0), std::move(input_data1), std::move(input_data2));
    }

    {
        auto input_data0 = std::make_unique<std::vector<int>>(std::vector{1, 2, 3, 4});
        auto input_data1 = std::make_unique<std::vector<int>>(std::vector{1, 2, 3, 4});
        auto input_data2 = std::make_unique<std::vector<int>>(std::vector{1, 2, 3, 4});
        dag_flower.Submit(std::move(input_data0), std::move(input_data1), std::move(input_data2));
    }

    {
        auto input_data0 = std::make_unique<std::vector<int>>(std::vector{1, 1, 1, 1});
        auto input_data1 = std::make_unique<std::vector<int>>(std::vector{1, 1, 1, 1});
        auto input_data2 = std::make_unique<std::vector<int>>(std::vector{1, 1, 1, 1});
        dag_flower.Submit(std::move(input_data0), std::move(input_data1), std::move(input_data2));
    }

    return 0;
}