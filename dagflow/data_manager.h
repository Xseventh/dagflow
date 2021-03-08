//
// Created by seventh on 2020/12/13.
//

#ifndef DAGFLOW_DAGFLOW_DATA_MANAGER_H_
#define DAGFLOW_DAGFLOW_DATA_MANAGER_H_

#include <vector>
#include <memory>

class DataManager {
  public:
    DataManager() = default;

    template<typename Type>
    size_t NewDataPtr(std::shared_ptr<std::shared_ptr<Type>> &p_data) {
        p_data = std::make_shared<std::shared_ptr<Type>>(nullptr);
        m_p_data_ptrs.emplace_back(p_data);
        return m_p_data_ptrs.size() - 1;
    }

    template<typename Type>
    std::shared_ptr<std::shared_ptr<Type>> Get(size_t id) const {
        auto p_data_ptr = m_p_data_ptrs[id].lock();
        if (!p_data_ptr) {
            // data_ptr has been released
            std::abort();
        }
        return std::static_pointer_cast<std::shared_ptr<Type>>(p_data_ptr);
    }
  private:
    std::vector<std::weak_ptr<void>> m_p_data_ptrs;
};

#endif //DAGFLOW_DAGFLOW_DATA_MANAGER_H_
