//
// Created by seventh on 2021/3/14.
//

#ifndef DAGFLOW_DAG_FLOWER_CONTEXT_H
#define DAGFLOW_DAG_FLOWER_CONTEXT_H

#include "dagflow/common/thread_pool.h"
#include "dagflow/dagflower/data_manager.h"
#include "dagflow/dagflower/executor/dag_executor_interface.h"

namespace dagflow::detail {

struct DagFlowerContext {
    DagFlowerContext(common::ThreadPool &thread_pool, size_t data_count) : mr_thread_pool(thread_pool),
                                                                           m_data_manager(data_count) {}

    common::ThreadPool &mr_thread_pool;
    std::vector<std::unique_ptr<IDagExecutor>> mp_executors;
    detail::DataManager m_data_manager;
};

}
#endif //DAGFLOW_DAG_FLOWER_CONTEXT_H
