//
// Created by seventh on 2021/3/12.
//

#ifndef DAGFLOW_DAG_DATA_H
#define DAGFLOW_DAG_DATA_H

#include <functional>
#include <memory>

#include "dagflow/common/template_helper.h"
#include "dag_data.inl"
#include "dag_flower_info_interface.h"

namespace dagflow {

template<typename... DataType>
template<typename ...OutputType>
DagData<DataType..., OutputType...>
DagData<DataType...>::operator|(const DagData<OutputType...> &dag_data) const noexcept {

    std::array<size_t, this->DataSize + dag_data.DataSize> data_id_list, node_id_list;

    std::copy(this->m_data_id_list.cbegin(), this->m_data_id_list.cend(), data_id_list.begin());
    std::copy(dag_data.m_data_id_list.cbegin(), dag_data.m_data_id_list.cend(), data_id_list.begin() + this->DataSize);

    std::copy(this->m_node_id_list.cbegin(), this->m_node_id_list.cend(), node_id_list.begin());
    std::copy(dag_data.m_node_id_list.cbegin(), dag_data.m_node_id_list.cend(), node_id_list.begin() + this->DataSize);

    return DagData<DataType..., OutputType...>(data_id_list, node_id_list, mr_dag_flower_info);
}

template<typename... DataType>
template<typename... OutputType>
DagData<OutputType...>
DagData<DataType...>::Flow(const common::block_deduction_t <SyncFuncType<OutputType...>> &function) const noexcept {
    return mr_dag_flower_info.AddNodeInfo(function, m_data_id_list, m_node_id_list,
                                          (std::tuple<DataType...> *) nullptr, (std::tuple<OutputType...> *) nullptr);
}

template<typename... DataType>
template<size_t idx>
DagData <common::nth_element_t<idx, DataType...>> DagData<DataType...>::Get() {
    return DagData<common::nth_element_t<idx, DataType...>>({m_data_id_list[idx]}, {m_node_id_list[idx]},
                                                            mr_dag_flower_info);
}

template<typename ...DataType>
DagData<DataType...>::DagData(const std::array<size_t, DataSize> &data_id_list, const size_t &node_id,
                              detail::IDagFlowerInfo &dag_flower_info)
        : m_data_id_list(data_id_list),
          m_node_id_list({((size_t) (DataType *) node_id)...}),
          mr_dag_flower_info(dag_flower_info) {}

template<typename ...DataType>
DagData<DataType...>::DagData(const std::array<size_t, DataSize> &data_id_list,
                              const std::array<size_t, DataSize> &node_id_list,
                              detail::IDagFlowerInfo &dag_flower_info)
        : m_data_id_list(data_id_list),
          m_node_id_list(node_id_list),
          mr_dag_flower_info(dag_flower_info) {}

template<typename ...DataType>
DagData<DataType...>::DagData(const DagData<DataType...> &dag_data, detail::IDagFlowerInfo &dag_flower_info)
        :m_data_id_list(dag_data.m_data_id_list),
         m_node_id_list(dag_data.m_node_id_list),
         mr_dag_flower_info(dag_flower_info) {}

DagData<>::DagData(const std::array<size_t, DataSize> &data_id_list, const size_t &node_id,
                   detail::IDagFlowerInfo &dag_flower_info)
        : m_data_id_list(data_id_list), m_node_id_list(),
          mr_dag_flower_info(dag_flower_info) {}

DagData<>::DagData(const std::array<size_t, DataSize> &data_id_list,
                   const std::array<size_t, DataSize> &node_id_list,
                   detail::IDagFlowerInfo &dag_flower_info)
        : m_data_id_list(data_id_list), m_node_id_list(node_id_list), mr_dag_flower_info(dag_flower_info) {}

DagData<>::DagData(const DagData<> &dag_data, detail::IDagFlowerInfo &dag_flower_info)
        : m_data_id_list(dag_data.m_data_id_list),
          m_node_id_list(dag_data.m_node_id_list),
          mr_dag_flower_info(dag_flower_info) {}


}

#endif //DAGFLOW_DAG_DATA_H
