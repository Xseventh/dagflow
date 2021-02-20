//
// Created by seventh on 2020/12/13.
//

#ifndef DAGFLOW_DAGFLOW_RUNNER_IMPL_H_
#define DAGFLOW_DAGFLOW_RUNNER_IMPL_H_
#include "runner.h"
namespace dagflow {

template<typename ...InputTypes>
void IRunner<InputTypes...>::Submit(DataPtr<InputTypes>... inputs) {
    mSubmit(std::move(inputs.mp_data)...);
}

template<typename T>
struct DataPtr {
    DataPtr(std::unique_ptr<T> &&p_data) : mp_data(p_data.release()) {}

    DataPtr(std::shared_ptr<T> p_data) : mp_data(std::move(p_data)) {}

    DataPtr(T *const &p_data) : mp_data(p_data, [](T *) {}) {}
    std::shared_ptr<T> mp_data;
};

}
#endif //DAGFLOW_DAGFLOW_RUNNER_IMPL_H_
