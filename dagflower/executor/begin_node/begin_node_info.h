//
// Created by seventh on 2021/3/16.
//

#ifndef DAGFLOW_BEGIN_NODE_INFO_H
#define DAGFLOW_BEGIN_NODE_INFO_H

#include "dagflow/dagflower/executor/dag_executor_info_interface.h"
#include "begin_node.h"

namespace dagflow::detail {

template<typename T0, typename T1>
class BeginNodeInfoImpl;

template<typename... InputType>
using BeginNodeInfo = BeginNodeInfoImpl<std::tuple<InputType...>, std::make_index_sequence<sizeof...(InputType)>>;

template<typename... InputType, size_t ...InputIndex>
class BeginNodeInfoImpl<std::tuple<InputType...>, std::index_sequence<InputIndex...>> : public IDagExecutorInfo {
public:
    BeginNodeInfoImpl(const std::array<size_t, sizeof...(InputType)> &input_data_id)
            : IDagExecutorInfo(std::array<size_t, 0>{}),
              m_input_data_id(input_data_id) {}

    BeginNodeInfoImpl(
            const BeginNodeInfoImpl<std::tuple<InputType...>, std::index_sequence<InputIndex...>> &) = default;

    std::unique_ptr<IDagExecutorInfo>
    CopyDagExecutorInfo() override {
        return std::make_unique<BeginNodeInfo<InputType...>>(*this);
    }

    std::unique_ptr<IDagExecutor>
    NewDagExecutor(DataManager &data_manager, common::ThreadPool &thread_pool) override {
        return std::make_unique<BeginNode<InputType...>>(
                data_manager.Get<common::nth_element_t<InputIndex, InputType...>>(m_input_data_id[InputIndex])...,
                thread_pool);
    }

private:
    std::array<size_t, sizeof...(InputType)> m_input_data_id;
};

}

#endif //DAGFLOW_BEGIN_NODE_INFO_H
