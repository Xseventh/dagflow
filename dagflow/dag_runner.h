//
// Created by seventh on 2021/2/28.
//

#ifndef DAGFLOW_DAG_RUNNER_H
#define DAGFLOW_DAG_RUNNER_H

#include <thread>

#include "dagflow/common/thread_pool.h"

#include "runner.h"
#include "data_manager.h"
#include "node.h"
#include "dag_data.h"

namespace dagflow {

template<typename ...InputTypes>
class DagRunner : public IRunner<InputTypes...> {
public:
    explicit DagRunner(size_t thread_num = std::thread::hardware_concurrency())
            : DagRunner(thread_num, std::make_index_sequence<sizeof...(InputTypes)>{}) {std::cout << "thread_num : " << thread_num << std::endl;}

    // deleted
    DagRunner(const DagRunner &) = delete;

    DagRunner(DagRunner &&) = delete;

    DagRunner &operator=(const DagRunner &) = delete;

    DagRunner &operator=(DagRunner &&) = delete;

    ~DagRunner() {
        m_thread_pool.stop(true);
    }

private:

    template<size_t... Indexes>
    DagRunner(size_t thread_num, std::index_sequence<Indexes...>)
            : m_thread_pool(thread_num),
              mpp_input_data({m_data_manager.NewDataPtr<std::tuple_element_t<Indexes, std::tuple<InputTypes...>>>(
                      m_input_data_id[Indexes])...}),
              m_dag_data_builder(m_node_info, m_node_depended_id, m_data_manager),
              m_source_data(m_input_data_id, {/* 0 is source node id */(Indexes * 0)...}, m_dag_data_builder),
              m_node_depended_id(1) {

        m_node_info.emplace_back(
                std::make_unique<NodeInfo>(std::make_unique<Node<std::tuple<>, std::tuple<InputTypes...>>>(
                        [this](std::shared_ptr<InputTypes> &... inputs) {
                            std::forward_as_tuple(inputs...) = std::forward_as_tuple(
                                    *std::get<Indexes>(this->mpp_input_data)...);
                        },
                        std::get<Indexes>(mpp_input_data)...
                ), 0, std::vector<size_t>{}, m_node_depended_id));
    }

    void mSubmit(std::shared_ptr<InputTypes>... inputs) override {
        std::apply(GetDataPtr<std::shared_ptr<InputTypes> &...>, mpp_input_data) = std::forward_as_tuple(
                std::move(inputs)...);
        RunNode(0);
    }


private:

    void RunNode(size_t node_id) {
        m_thread_pool.push([this, node_id](int thread_runner_id) {
            m_node_info[node_id]->m_node->Run();
            for (const size_t &depended_node_id : m_node_depended_id[node_id]) {
                size_t dependent_count = --m_node_info[depended_node_id]->m_node_dependent_count;
                if (dependent_count == 0) {
                    RunNode(depended_node_id);
                }
            }
        });
    }

    DataManager m_data_manager;


    std::array<size_t, sizeof...(InputTypes)> m_input_data_id;
    std::tuple<std::shared_ptr<std::shared_ptr<InputTypes>>...> mpp_input_data;
    std::vector<std::vector<size_t>> m_node_depended_id;
    std::vector<std::unique_ptr<NodeInfo>> m_node_info;

    common::ThreadPool m_thread_pool;
    DagDataBuilder m_dag_data_builder;

public:
    DagData<InputTypes...> m_source_data;
};

}

#endif //DAGFLOW_DAG_RUNNER_H
