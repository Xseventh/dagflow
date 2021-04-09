//
// Created by seventh on 2021/3/12.
//

#ifndef DAGFLOW_NODE_INFO_H
#define DAGFLOW_NODE_INFO_H

#include <tuple>

#include "dagflow/dagflower/executor/dag_executor_info_interface.h"
#include "node.h"

namespace dagflow::detail {

template<typename T0, typename T1, typename T2, typename T3>
class NodeInfoImpl;

template<typename InputType, typename OutputType>
struct NodeInfoAlias;

template<typename... InputType, typename... OutputType>
struct NodeInfoAlias<std::tuple<InputType...>, std::tuple<OutputType...>> {
    using type = NodeInfoImpl<std::tuple<InputType...>, std::make_index_sequence<sizeof...(InputType)>, std::tuple<OutputType...>, std::make_index_sequence<sizeof...(OutputType)>>;
};

template<typename... Type>
using NodeInfo = typename NodeInfoAlias<Type...>::type;

template<typename ...InputType, size_t ...InputIndex, typename ...OutputType, size_t ...OutputIndex>
class NodeInfoImpl<std::tuple<InputType...>, std::index_sequence<InputIndex...>, std::tuple<OutputType...>, std::index_sequence<OutputIndex...>>
        : public IDagExecutorInfo {
public:
    using SyncFuncType = std::function<void(const std::shared_ptr<InputType> &..., std::shared_ptr<OutputType> &...)>;

    template<size_t Length>
    NodeInfoImpl(SyncFuncType function,
                 const std::array<size_t, sizeof...(InputType)> &input_data_id,
                 const std::array<size_t, sizeof...(OutputType)> &output_data_id,
                 const std::array<size_t, Length> &dependent_node)
            : m_function(std::move(function)),
              m_input_data_id(input_data_id),
              m_output_data_id(output_data_id),
              IDagExecutorInfo(dependent_node) {}

    NodeInfoImpl(
            const NodeInfoImpl<std::tuple<InputType...>, std::index_sequence<InputIndex...>, std::tuple<OutputType...>, std::index_sequence<OutputIndex...>> &) = default;

    std::unique_ptr<IDagExecutorInfo>
    CopyDagExecutorInfo() override {
        return std::make_unique<NodeInfo<std::tuple<InputType...>, std::tuple<OutputType...>>>(*this);
    }

    std::unique_ptr<IDagExecutor>
    NewDagExecutor(DataManager &data_manager, common::ThreadPool &thread_pool) override {
        return std::make_unique<Node<std::tuple<InputType...>, std::tuple<OutputType...>>>(
                m_function,
                data_manager.Get<common::nth_element_t<InputIndex, InputType...>>(m_input_data_id[InputIndex])...,
                data_manager.Get<common::nth_element_t<OutputIndex, OutputType...>>(m_output_data_id[OutputIndex])...,
                m_dependent_node_id.size(), thread_pool);
    }

private:
    SyncFuncType m_function;
    std::array<size_t, sizeof...(InputType)> m_input_data_id;
    std::array<size_t, sizeof...(OutputType)> m_output_data_id;
};

}

#endif //DAGFLOW_NODE_INFO_H
