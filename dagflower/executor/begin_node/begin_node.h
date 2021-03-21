//
// Created by seventh on 2021/3/16.
//

#ifndef DAGFLOW_BEGIN_NODE_H
#define DAGFLOW_BEGIN_NODE_H

#include "dagflow/dagflower/executor/dag_executor_info_interface.h"

namespace dagflow::detail {

template<typename T0, typename T1>
class BeginNodeImpl;

template<typename... InputType>
using BeginNode = BeginNodeImpl<std::tuple<InputType...>, std::make_index_sequence<sizeof...(InputType)>>;

template<typename... InputType, size_t ...InputIndex>
class BeginNodeImpl<std::tuple<InputType...>, std::index_sequence<InputIndex...>> : public IDagExecutor {
public:

    explicit BeginNodeImpl(std::shared_ptr<std::shared_ptr<InputType>>... p_input_ptr,
                           common::ThreadPool &r_thread_pool)
            : IDagExecutor(0, r_thread_pool), m_input_data(std::make_tuple(std::move(p_input_ptr)...)) {}


    void Submit(std::shared_ptr<InputType> ...inputs) {
        std::forward_as_tuple(*std::get<InputIndex>(m_input_data)...) = std::forward_as_tuple(inputs...);
        (std::get<InputIndex>(m_input_data).reset(), ...);

        Run();

    }

    void Run() override {
        // update dependent executor
        IDagExecutor::UpdateDependentOnExecutor();
    }

private:
    std::tuple<std::shared_ptr<std::shared_ptr<InputType>>...> m_input_data;
};

}


#endif //DAGFLOW_BEGIN_NODE_H
