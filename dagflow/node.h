//
// Created by seventh on 2020/12/9.
//

#ifndef DAGFLOW_NODE_H_
#define DAGFLOW_NODE_H_

#include <tuple>
#include <functional>
#include "runner.h"
#include "data_manager.h"

namespace dagflow {

template<typename ... Args>
std::tuple<Args...> GetDataPtr(const std::shared_ptr<std::remove_reference_t<Args>> &... args) {
    return std::forward_as_tuple((*args)...);
}

class INode {
public:
    virtual void Run() const = 0;
};

template<typename InputType, typename OutputType>
class Node;

template<typename ... InputTypes, typename ... OutputTypes>
class Node<std::tuple<InputTypes...>, std::tuple<OutputTypes...>> : public INode {
public:
    using SyncFuncType = std::function<void(const std::shared_ptr<InputTypes> &..., std::shared_ptr<OutputTypes> &...)>;

    Node(SyncFuncType function,
         std::shared_ptr<std::shared_ptr<InputTypes>>... p_input_ptr,
         std::shared_ptr<std::shared_ptr<OutputTypes>>... p_output_ptr)
            : m_function(std::move(function)),
              m_input_data(std::make_tuple(std::move(p_input_ptr)...)),
              m_output_data(std::make_tuple(std::move(p_output_ptr)...)) {}


    void Run() const override {
        std::apply(m_function,
                   std::tuple_cat(std::apply(GetDataPtr<const std::shared_ptr<InputTypes> &...>, m_input_data),
                                  std::apply(GetDataPtr < std::shared_ptr<OutputTypes> & ... > , m_output_data)));
    }

private:

    SyncFuncType m_function;
    std::tuple<std::shared_ptr<std::shared_ptr<InputTypes>>...> m_input_data;
    std::tuple<std::shared_ptr<std::shared_ptr<OutputTypes>>...> m_output_data;
};

}
#endif //DAGFLOW_NODE_H_
