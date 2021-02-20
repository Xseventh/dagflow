//
// Created by seventh on 2020/12/9.
//

#ifndef DAGFLOW_NODE_H_
#define DAGFLOW_NODE_H_

#include <tuple>
#include <functional>
#include "runner.h"
#include "data_manager.h"
#include "data.h"

namespace dagflow {

template<typename InputType, typename OutputType>
class Node;

template<typename ... InputTypes, typename ... OuputTypes>
class Node<std::tuple<InputTypes...>, std::tuple<OuputTypes...>> : public IRunner<InputTypes...> {
public:
    using FuncType = std::function<void(const std::shared_ptr<InputTypes> &..., std::shared_ptr<OuputTypes> &...)>;

    Node(FuncType function, DataManager &data_manager)
            : Node(std::move(function), data_manager, std::make_index_sequence<sizeof...(OuputTypes)>{}) {}

    template<size_t... Indexes>
    Node(FuncType function, DataManager &data_manager, std::index_sequence<Indexes...>)
            : m_function(std::move(function)), m_data_manager(data_manager),
              m_output_data_id{data_manager.NewDataPtr(std::get<Indexes>(m_output_data))...} {}

private:
    template<typename ... Args>
    static std::tuple<Args...> InputTransform(Args... args) {
        return std::forward_as_tuple(args...);
    }

    template<typename ... Args>
    static std::tuple<Args...> OutputTransform(std::shared_ptr<std::remove_reference_t<Args>>... args) {
        return std::forward_as_tuple((*args)...);
    }

    void mSubmit(std::shared_ptr<InputTypes>... inputs) override {
        std::apply(m_function, std::tuple_cat(InputTransform<const std::shared_ptr<InputTypes> &...>(inputs...),
                                              std::apply(OutputTransform<std::shared_ptr<OuputTypes> &...>,
                                                         m_output_data)));
    }

    FuncType m_function;
    DataManager &m_data_manager;
    std::tuple<std::shared_ptr<std::shared_ptr<OuputTypes>>...> m_output_data;
    const size_t m_output_data_id[sizeof...(OuputTypes)];
};

}
#endif //DAGFLOW_NODE_H_
