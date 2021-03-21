//
// Created by seventh on 2021/3/14.
//

#ifndef DAGFLOW_END_NODE_H
#define DAGFLOW_END_NODE_H

#include <tuple>

#include "dagflow/dagflower/executor/dag_executor_info_interface.h"
#include "dagflow/dagflower/dag_flower_context.h"

namespace dagflow::detail {

class EndNode : public IDagExecutor {
public:

    EndNode(const size_t &wait_number,
            std::unique_ptr<dagflow::detail::DagFlowerContext> &&p_context,
            common::ThreadPool &r_thread_pool)
            : IDagExecutor(wait_number, r_thread_pool),
              mp_context(std::move(p_context)) {}


    void Run() override {
        mp_context.reset();
    }

private:
    std::unique_ptr<dagflow::detail::DagFlowerContext> mp_context;
};

}

#endif //DAGFLOW_END_NODE_H
