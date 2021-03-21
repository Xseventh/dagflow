//
// Created by seventh on 2021/3/12.
//

#ifndef DAGFLOW_DAG_EXECUTOR_INFO_INTERFACE_H
#define DAGFLOW_DAG_EXECUTOR_INFO_INTERFACE_H

#include <memory>

#include "dagflow/common/thread_pool.h"
#include "dagflow/dagflower/executor/dag_executor_interface.h"
#include "dagflow/dagflower/data_manager.h"

namespace dagflow::detail {

class IDagExecutorInfo {
public:
    template<size_t Length>
    explicit IDagExecutorInfo(const std::array<size_t, Length> &dependent_node)
            : m_dependent_node_id(dependent_node.begin(), dependent_node.end()) {
        std::sort(m_dependent_node_id.begin(), m_dependent_node_id.end());
        m_dependent_node_id.erase(std::unique(m_dependent_node_id.begin(), m_dependent_node_id.end()),
                                  m_dependent_node_id.end());
    }

    explicit IDagExecutorInfo(const std::vector<size_t> &dependent_node)
            : m_dependent_node_id(dependent_node.begin(), dependent_node.end()) {
        std::sort(m_dependent_node_id.begin(), m_dependent_node_id.end());
        m_dependent_node_id.erase(std::unique(m_dependent_node_id.begin(), m_dependent_node_id.end()),
                                  m_dependent_node_id.end());
    }

    virtual std::unique_ptr<IDagExecutor>
    NewDagExecutor(DataManager &data_manager, common::ThreadPool &thread_pool) = 0;

    virtual ~IDagExecutorInfo() = default;

    std::vector<size_t> m_dependent_node_id;
};

}
#endif //DAGFLOW_DAG_EXECUTOR_INFO_INTERFACE_H
