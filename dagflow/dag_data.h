//
// Created by seventh on 2021/2/23.
//

#ifndef DAGFLOW_DAG_DATA_H
#define DAGFLOW_DAG_DATA_H

#include <memory>
#include <vector>
#include <functional>

#include "dagflow/common/template_helper.h"

namespace dagflow {

template<typename ...InputTypes>
class DagRunner;

class DagDataBuilder;

struct NodeInfo {
    NodeInfo(std::unique_ptr<INode> node, const size_t &node_id, const std::vector<size_t> &dependent_id_list,
             std::vector<std::vector<size_t>> &r_node_depended_id)
            : m_node(std::move(node)), m_node_dependent_count(dependent_id_list.size()), m_node_id(node_id),
              mr_node_depended_id(r_node_depended_id) {
        for (const auto &dependent_id: dependent_id_list) {
            mr_node_depended_id[dependent_id].emplace_back(m_node_id);
        }
        mr_node_depended_id.emplace_back();
    }

    size_t m_node_id;
    std::unique_ptr<INode> m_node;
    std::atomic<size_t> m_node_dependent_count;
    std::vector<std::vector<size_t>> &mr_node_depended_id;
};

template<typename ...Types>
class DagData {
public:
    template<typename ...OTypes>
    using SyncFuncType = std::function<void(const std::shared_ptr<Types> &..., std::shared_ptr<OTypes> &...)>;
    static constexpr size_t DataSize = sizeof...(Types);


    template<typename ...OTypes>
    DagData<OTypes...> operator>>=(const common::block_deduction_t<SyncFuncType<OTypes...>> &) noexcept;

    template<typename ...OTypes>
    DagData<Types..., OTypes...> operator|(const DagData<OTypes...> &) noexcept;

    template<size_t idx>
    DagData<std::tuple_element_t<idx, std::tuple<Types...>>> Get();

private:
    template<typename ...OTypes>
    friend
    class DagData;

    template<typename ...InputTypes>
    friend
    class DagRunner;

    friend class DagDataBuilder;

    DagData(const std::array<size_t, DataSize> &data_id_list, const std::array<size_t, DataSize> &node_id_list,
            const DagDataBuilder &data_builder)
            : m_data_id(data_id_list), m_node_id(node_id_list), mr_data_builder(data_builder) {}

    const std::array<size_t, DataSize> m_data_id;
    const std::array<size_t, DataSize> m_node_id;
    const DagDataBuilder &mr_data_builder;
};

template<>
class DagData<> {
    static constexpr size_t DataSize = 0;
private:
    template<typename ...OTypes>
    friend
    class DagData;

    template<typename ...InputTypes>
    friend
    class DagRunner;

    friend class DagDataBuilder;

    DagData(const std::array<size_t, DataSize> &data_id_list, const std::array<size_t, DataSize> &node_id_list,
            const DagDataBuilder &data_builder)
            : m_data_id(data_id_list), m_node_id(node_id_list), mr_data_builder(data_builder) {}

    const std::array<size_t, DataSize> m_data_id;
    const std::array<size_t, DataSize> m_node_id;
    const DagDataBuilder &mr_data_builder;
};

class DagDataBuilder {
public:
    DagDataBuilder(std::vector<std::unique_ptr<NodeInfo>> &node_info,
                   std::vector<std::vector<size_t>> &node_depended_id,
                   DataManager &data_manager)
            : mr_node_info(node_info),
              mr_node_depended_id(node_depended_id),
              mr_data_manager(data_manager) {}

    template<typename ...InputType, typename ...OutputType>
    DagData<OutputType...> GetNewDagData(const DagData<InputType...> &dag_data,
                                         std::tuple<OutputType...> *,
                                         const typename DagData<InputType...>::template SyncFuncType<OutputType...> &run_function) const {
        return GetNewDagData(dag_data, (std::tuple<OutputType...> *) nullptr, run_function,
                             std::make_index_sequence<sizeof...(InputType)>{},
                             std::make_index_sequence<sizeof...(OutputType)>{});
    }

    template<typename ...InputType, typename ...OutputType, size_t... InputIndexes, size_t... OutputIndexes>
    DagData<OutputType...> GetNewDagData(const DagData<InputType...> &dag_data,
                                         std::tuple<OutputType...> *,
                                         const typename DagData<InputType...>::template SyncFuncType<OutputType...> &run_function,
                                         std::index_sequence<InputIndexes...>,
                                         std::index_sequence<OutputIndexes...>) const {
        size_t node_id = mr_node_info.size();
        std::vector<size_t> dependent_id_list(dag_data.m_node_id.cbegin(), dag_data.m_node_id.cend());
        // id deduplication
        std::sort(dependent_id_list.begin(), dependent_id_list.end());
        dependent_id_list.erase(std::unique(dependent_id_list.begin(), dependent_id_list.end()),
                                dependent_id_list.end());

        std::tuple<std::shared_ptr<std::shared_ptr<InputType>>...> input_data{
                mr_data_manager.Get<std::tuple_element_t<InputIndexes, std::tuple<InputType...>>>(
                        dag_data.m_data_id[InputIndexes])...};

        std::array<size_t, sizeof...(OutputType)> output_data_id;
        std::tuple<std::shared_ptr<std::shared_ptr<OutputType>>...> output_data{
                mr_data_manager.NewDataPtr<std::tuple_element_t<OutputIndexes, std::tuple<OutputType...>>>(
                        output_data_id[OutputIndexes])...};

        mr_node_info.emplace_back(
                std::make_unique<NodeInfo>(std::make_unique<Node<std::tuple<InputType...>, std::tuple<OutputType...>>>(
                        run_function,
                        std::move(std::get<InputIndexes>(input_data))...,
                        std::move(std::get<OutputIndexes>(output_data))...
                ), node_id, dependent_id_list, mr_node_depended_id));

        return DagData<OutputType...>(output_data_id,
                                      {(OutputIndexes * 0 + node_id)...},
                                      *this);
    }

private:

    DataManager &mr_data_manager;
    std::vector<std::unique_ptr<NodeInfo>> &mr_node_info;
    std::vector<std::vector<size_t>> &mr_node_depended_id;
};

template<typename... Types>
template<typename... OTypes>
DagData<OTypes...>
DagData<Types...>::operator>>=(const common::block_deduction_t<SyncFuncType<OTypes...>> &run_function) noexcept {
    return mr_data_builder.GetNewDagData(*this, (std::tuple<OTypes...> *) nullptr, run_function);
}

template<typename... Types>
template<typename... OTypes>
DagData<Types..., OTypes...>
DagData<Types...>::operator|(const DagData<OTypes...> &dag_data) noexcept {
    std::array<size_t, this->DataSize + dag_data.DataSize> data_id, node_id;

    std::copy(this->m_data_id.cbegin(), this->m_data_id.cend(), data_id.begin());
    std::copy(dag_data.m_data_id.cbegin(), dag_data.m_data_id.cend(), data_id.begin() + this->DataSize);

    std::copy(this->m_node_id.cbegin(), this->m_node_id.cend(), node_id.begin());
    std::copy(dag_data.m_node_id.cbegin(), dag_data.m_node_id.cend(), node_id.begin() + this->DataSize);

    return DagData<Types..., OTypes...>(data_id, node_id, mr_data_builder);
}

template<typename... Types>
template<size_t idx>
DagData<std::tuple_element_t<idx, std::tuple<Types...>>> DagData<Types...>::Get() {
    return DagData<std::tuple_element_t<idx, std::tuple<Types...>>>({m_data_id[idx]}, {m_node_id[idx]},
                                                                    mr_data_builder);
}

} // namespace dagflow
#endif //DAGFLOW_DAG_DATA_H
