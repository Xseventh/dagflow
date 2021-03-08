//
// Created by seventh on 2020/12/6.
//

#ifndef DAGFLOW_RUNNER_H_
#define DAGFLOW_RUNNER_H_
#include <type_traits>
#include <memory>
namespace dagflow {

template<typename T>
struct DataPtr;

template<typename ...InputTypes>
class IRunner {
  public:

    void Submit(DataPtr<InputTypes>... inputs);
  private:
    virtual void mSubmit(std::shared_ptr<InputTypes>... inputs) = 0;
};

}

#include "runner_impl.h"
#endif //DAGFLOW_RUNNER_H_
