//
// Created by seventh on 2021/3/18.
//

#ifndef DAGFLOW_DAG_DATA_INL
#define DAGFLOW_DAG_DATA_INL

#include <functional>
#include <memory>
#include "dagflow/common/template_helper.h"


namespace dagflow {
template<typename ...DataType>
class DagData;
}

#include "dag_flower_info_interface.inl"

namespace dagflow {

template<typename... DataType>
class DagData {
public:
    template<typename ...OutputType>
    using SyncFuncType = std::function<void(const std::shared_ptr<DataType> &..., std::shared_ptr<OutputType> &...)>;

    static constexpr size_t DataSize = sizeof...(DataType);

    template<typename ...OutputType>
    DagData<OutputType...> Flow(const common::block_deduction_t<SyncFuncType<OutputType...>> &function) const noexcept;

    template<typename ...OutputType>
    DagData<DataType..., OutputType...> operator|(const DagData<OutputType...> &) const noexcept;

    template<size_t idx>
    DagData<common::nth_element_t<idx, DataType...>> Get();

private:


    template<typename... Type> friend
    class DagData;

    friend detail::IDagFlowerInfo;

    template<typename... InputType> friend
    class DagFlowerInfo;

    DagData(const std::array<size_t, DataSize> &data_id_list, const size_t &node_id,
            detail::IDagFlowerInfo &dag_flower_info);

    DagData(const std::array<size_t, DataSize> &data_id_list, const std::array<size_t, DataSize> &node_id_list,
            detail::IDagFlowerInfo &dag_flower_info);

    // for Info Copy Constructor
    DagData(const DagData<DataType...>& dag_data, detail::IDagFlowerInfo &dag_flower_info);

    detail::IDagFlowerInfo &mr_dag_flower_info;
    const std::array<size_t, DataSize> m_data_id_list;
    const std::array<size_t, DataSize> m_node_id_list;
};


template<>
class DagData<> {
public:
    static constexpr size_t DataSize = 0;

private:


    template<typename... Type> friend
    class DagData;

    friend detail::IDagFlowerInfo;

    template<typename... InputType> friend
    class DagFlowerInfo;

    DagData(const std::array<size_t, DataSize> &data_id_list, const size_t &node_id,
            detail::IDagFlowerInfo &dag_flower_info);

    DagData(const std::array<size_t, DataSize> &data_id_list, const std::array<size_t, DataSize> &node_id_list,
            detail::IDagFlowerInfo &dag_flower_info);

    // for Info Copy Constructor
    DagData(const DagData<> &dag_data, detail::IDagFlowerInfo &dag_flower_info);

    detail::IDagFlowerInfo &mr_dag_flower_info;
    const std::array<size_t, DataSize> m_data_id_list;
    const std::array<size_t, DataSize> m_node_id_list;
};

}

#endif //DAGFLOW_DAG_DATA_INL
