//
// Created by seventh on 2021/3/12.
//

#ifndef DAGFLOW_DAG_EXECUTOR_INTERFACE_H
#define DAGFLOW_DAG_EXECUTOR_INTERFACE_H

#include <vector>
#include <atomic>

#include "dagflow/common/thread_pool.h"

namespace dagflow::detail {

class IDagExecutor {
public:
    explicit IDagExecutor(const size_t &wait_count, common::ThreadPool &r_thread_pool)
            : m_wait_count(wait_count),
              mr_thread_pool(r_thread_pool) {}

    virtual void Run() = 0;

    void AppendDependentOnExecutor(IDagExecutor *p_dep_executor) {
        mp_dependent_on_executors.emplace_back(p_dep_executor);
    }

    bool IsEndExecutor() {
        return mp_dependent_on_executors.empty();
    }

    virtual ~IDagExecutor() {};

protected:

    void UpdateDependentOnExecutor() {
        for (const auto &p_executor : mp_dependent_on_executors) {
            size_t dependent_count = --p_executor->m_wait_count;
            if (dependent_count == 0) {
                mr_thread_pool.push([this, p_executor](int thread_idx) { p_executor->Run(); });
            }
        }
    }

private:

    std::atomic<size_t> m_wait_count;
    std::vector<IDagExecutor *> mp_dependent_on_executors;
    common::ThreadPool &mr_thread_pool;
};

}

#endif //DAGFLOW_DAG_EXECUTOR_INTERFACE_H
