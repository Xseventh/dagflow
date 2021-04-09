//
// Created by seventh on 2021/3/12.
//

#ifndef DAGFLOW_DAG_FLOWER_INFO_H
#define DAGFLOW_DAG_FLOWER_INFO_H

#include <vector>

#include "dag_flower_info_interface.h"
#include "dagflow/dagflower/executor/begin_node/begin_node_info.h"

namespace dagflow {

template<typename... InputType>
class DagFlowerInfo : public detail::IDagFlowerInfo {
public:
    DagFlowerInfo() : input_dag_data(AddInputNodeInfo()) {}

    DagFlowerInfo(DagFlowerInfo<InputType...> &&dag_info)
            : detail::IDagFlowerInfo(std::move(dag_info)), input_dag_data(std::move(dag_info.input_dag_data), *this) {}

    DagFlowerInfo(const DagFlowerInfo<InputType...> &dag_info)
            : detail::IDagFlowerInfo(dag_info), input_dag_data(dag_info.input_dag_data, *this) {}

    DagData<InputType...> GetInputData() const noexcept {
        return input_dag_data;
    }

private:

    DagData<InputType...>
    AddInputNodeInfo() {
        std::array<size_t, sizeof...(InputType)> input_id_list;
        for (auto &id :input_id_list) {
            id = m_data_id_generator++;
        }
        m_dag_executor_infos.emplace_back(std::make_unique<detail::BeginNodeInfo<InputType...>>(input_id_list));
        return DagData<InputType...>(input_id_list, m_dag_executor_infos.size() - 1, *this);
    }

    DagData<InputType...> input_dag_data;
};

}

#endif //DAGFLOW_DAG_FLOWER_INFO_H
