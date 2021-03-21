//
// Created by seventh on 2021/3/18.
//

#ifndef DAGFLOW_DAG_FLOWER_INFO_INTERFACE_INL
#define DAGFLOW_DAG_FLOWER_INFO_INTERFACE_INL

#include <functional>
#include <memory>

#include "dagflow/dagflower/executor/dag_executor_info_interface.h"

namespace dagflow::detail {
class IDagFlowerInfo;
}

#include "dag_data.inl"

namespace dagflow::detail {

class IDagFlowerInfo {
public:
    template<typename... DataType, typename ...OutputType>
    DagData<OutputType...>
    AddNodeInfo(
            const common::block_deduction_t<std::function<void(const std::shared_ptr<DataType> &...,
                                                               std::shared_ptr<OutputType> &...)>> &function,
            const std::array<size_t, sizeof...(DataType)> &data_id_list,
            const std::array<size_t, sizeof...(DataType)> &node_id_list,
            std::tuple<DataType...> *, // for help template deduction
            std::tuple<OutputType...> *);


    size_t m_data_id_generator = 0; // data_id [0,n)
    std::vector<std::unique_ptr<detail::IDagExecutorInfo>> m_dag_executor_infos; // node_id [0,n)
};

}

#endif //DAGFLOW_DAG_FLOWER_INFO_INTERFACE_INL
