//
// Created by seventh on 2020/12/13.
//

#ifndef DAGFLOW_DAGFLOW_DATA_MANAGER_H_
#define DAGFLOW_DAGFLOW_DATA_MANAGER_H_

#include <vector>
#include <memory>

namespace dagflow::detail {

class DataManager {
public:
    DataManager(size_t n) : m_p_data_ptrs(n) {}

    template<typename Type>
    std::shared_ptr<std::shared_ptr<Type>> Get(size_t data_id) {
        auto p_data_ptr = m_p_data_ptrs[data_id].lock();
        if (!p_data_ptr) {
            auto p_data = std::make_shared<std::shared_ptr<Type>>(nullptr);
            m_p_data_ptrs[data_id] = p_data;
            return p_data;
        }
        return std::static_pointer_cast<std::shared_ptr<Type>>(p_data_ptr);
    }

private:
    std::vector<std::weak_ptr<void>> m_p_data_ptrs;
};

}

#endif //DAGFLOW_DAGFLOW_DATA_MANAGER_H_
