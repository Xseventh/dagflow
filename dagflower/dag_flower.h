//
// Created by seventh on 2021/3/13.
//

#ifndef DAGFLOW_DAG_FLOWER_H
#define DAGFLOW_DAG_FLOWER_H

#include "dagflow/dagflower/dagflower_info/dag_data.h"
#include "dagflow/dagflower/dagflower_info/dag_flower_info.h"
#include "dagflow/dagflower/dag_flower_context.h"
#include "dagflow/dagflower/executor/begin_node/begin_node.h"
#include "dagflow/dagflower/executor/end_node/end_node.h"


namespace dagflow {

namespace detail {
template<typename T>
struct DataPtr {
    DataPtr(std::unique_ptr<T> &&p_data) : mp_data(p_data.release()) {}

    DataPtr(std::shared_ptr<T> p_data) : mp_data(std::move(p_data)) {}

    DataPtr(T *const &p_data) : mp_data(p_data, [](T *) {}) {}

    std::shared_ptr<T> mp_data;
};

}

template<typename... InputType>
class DagFlower {
public:
    explicit DagFlower(DagFlowerInfo<InputType...> &&m_dag_info,
                       size_t thread_num = std::thread::hardware_concurrency())
            : m_thread_pool(thread_num),
              m_dag_info(std::move(m_dag_info)) {}


    void Submit(detail::DataPtr<InputType>... inputs) noexcept {
        std::unique_ptr<detail::DagFlowerContext> p_context = std::make_unique<detail::DagFlowerContext>(m_thread_pool,
                                                                                                         m_dag_info.m_data_id_generator);
        auto &thread_pool = p_context->mr_thread_pool;
        for (auto &p_executor_info : m_dag_info.m_dag_executor_infos) {
            p_context->mp_executors.emplace_back(
                    p_executor_info->NewDagExecutor(p_context->m_data_manager, thread_pool));
            for (auto node_id : p_executor_info->m_dependent_node_id) {
                p_context->mp_executors[node_id]->AppendDependentOnExecutor(p_context->mp_executors.back().get());
            }
        }

        auto &begin_node = static_cast<detail::BeginNode<InputType...> &>(*(p_context->mp_executors[0]));
        // End Node
        std::vector<size_t> dependent_node_id;
        for (size_t i = 0; i < p_context->mp_executors.size(); ++i) {
            if (p_context->mp_executors[i]->IsEndExecutor()) {
                dependent_node_id.emplace_back(i);
            }
        }
        p_context->mp_executors.emplace_back(
                std::make_unique<detail::EndNode>(dependent_node_id.size(), std::move(p_context), thread_pool));

        begin_node.Submit(std::move(inputs.mp_data)...);
    }

private:

    void Build() noexcept {

    }

    // source
    common::ThreadPool m_thread_pool;
    DagFlowerInfo<InputType...> m_dag_info;
};

}

#endif //DAGFLOW_DAG_FLOWER_H
