//
// Created by seventh on 2021/3/12.
//

#ifndef DAGFLOW_NODE_H
#define DAGFLOW_NODE_H

#include <tuple>
#include <memory>
#include <functional>

#include "dagflow/common/template_helper.h"
#include "dagflow/dagflower/executor/dag_executor_interface.h"

namespace dagflow::detail {


template<typename T0, typename T1, typename T2, typename T3>
class NodeImpl;


template<typename... InputType, size_t... InputIndex, typename... OutputType, size_t... OutputIndex>
class NodeImpl<std::tuple<InputType...>, std::index_sequence<InputIndex...>, std::tuple<OutputType...>, std::index_sequence<OutputIndex...>>
        : public IDagExecutor {
public:
    using SyncFuncType = std::function<void(const std::shared_ptr<InputType> &..., std::shared_ptr<OutputType> &...)>;

    NodeImpl(SyncFuncType function,
             std::shared_ptr<std::shared_ptr<InputType>>... p_input_ptr,
             std::shared_ptr<std::shared_ptr<OutputType>>... p_output_ptr,
             const size_t &wait_number, common::ThreadPool &r_thread_pool)
            : IDagExecutor(wait_number, r_thread_pool),
              m_function(std::move(function)),
              m_input_data(std::make_tuple(std::move(p_input_ptr)...)),
              m_output_data(std::make_tuple(std::move(p_output_ptr)...)) {}


    void Run() override {
        m_function(*std::get<InputIndex>(m_input_data)..., *std::get<OutputIndex>(m_output_data)...);
        (std::get<InputIndex>(m_input_data).reset(), ...);
        (std::get<OutputIndex>(m_output_data).reset(), ...);
        // update dependent executor
        IDagExecutor::UpdateDependentOnExecutor();
    }

private:

    SyncFuncType m_function;
    std::tuple<std::shared_ptr<std::shared_ptr<InputType>>...> m_input_data;
    std::tuple<std::shared_ptr<std::shared_ptr<OutputType>>...> m_output_data;
};

template<typename InputType, typename OutputType>
struct NodeAlias;

template<typename... InputType, typename... OutputType>
struct NodeAlias<std::tuple<InputType...>, std::tuple<OutputType...>> {
    using type = NodeImpl<std::tuple<InputType...>, std::make_index_sequence<sizeof...(InputType)>, std::tuple<OutputType...>, std::make_index_sequence<sizeof...(OutputType)>>;
};

template<typename... Type>
using Node = typename NodeAlias<Type...>::type;

}

#endif //DAGFLOW_NODE_H
