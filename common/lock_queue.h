//
// Created by seventh on 2020/12/6.
//

#ifndef DAGFLOW_COMMON_LOCK_QUEUE_H_
#define DAGFLOW_COMMON_LOCK_QUEUE_H_

#include <queue>
#include <mutex>

namespace dagflow::common {

template<typename T>
class LockQueue {
public:
    bool push(T const &value) {
        std::lock_guard<std::mutex> lock(this->mutex);
        this->q.push(value);
        return true;
    }

    // deletes the retrieved element, do not use for non integral types
    bool pop(T &v) {
        std::lock_guard<std::mutex> lock(this->mutex);
        if (this->q.empty())
            return false;
        v = std::move(this->q.front());
        this->q.pop();
        return true;
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(this->mutex);
        return this->q.empty();
    }

private:
    std::queue<T> q;
    std::mutex mutex;
};

}
#endif //DAGFLOW_COMMON_LOCK_QUEUE_H_
