//
// Created by seventh on 2021/3/18.
//

#ifndef DAGFLOW_DAG_FLOWER_INFO_INTERFACE_H
#define DAGFLOW_DAG_FLOWER_INFO_INTERFACE_H

#include <functional>
#include <memory>

#include "dag_flower_info_interface.inl"
#include "dag_data.h"
#include "dagflow/dagflower/executor/node/node_info.h"

namespace dagflow::detail {

template<typename... DataType, typename ...OutputType>
DagData<OutputType...> IDagFlowerInfo::AddNodeInfo(
        const common::block_deduction_t<std::function<void(const std::shared_ptr<DataType> &...,
                                                           std::shared_ptr<OutputType> &...)>> &function,
        const std::array<size_t, sizeof...(DataType)> &data_id_list,
        const std::array<size_t, sizeof...(DataType)> &node_id_list,
        std::tuple<DataType...> *, // for help template deduction
        std::tuple<OutputType...> *) {
    std::array<size_t, sizeof...(OutputType)> output_id_list;
    for (auto &id :output_id_list) {
        id = m_data_id_generator++;
    }
    m_dag_executor_infos.emplace_back(
            std::make_unique<detail::NodeInfo<std::tuple<DataType...>, std::tuple<OutputType...>>>(
                    function,
                    data_id_list,
                    output_id_list,
                    node_id_list));
    return DagData<OutputType...>(output_id_list, m_dag_executor_infos.size() - 1, *this);
}

}


#endif //DAGFLOW_DAG_FLOWER_INFO_INTERFACE_H
