//
// Created by seventh on 2020/12/6.
//

#ifndef DAGFLOW_COMMON_ThreadPool_H_
#define DAGFLOW_COMMON_ThreadPool_H_

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <type_traits>
#include <vector>

#include "lock_queue.h"

namespace dagflow::common {

class ThreadPool {

public:

    ThreadPool() { this->init(); }

    explicit ThreadPool(size_t nThreads) {
        this->init();
        this->resize(nThreads);
    }

    // deleted
    ThreadPool(const ThreadPool &) = delete;

    ThreadPool(ThreadPool &&) = delete;

    ThreadPool &operator=(const ThreadPool &) = delete;

    ThreadPool &operator=(ThreadPool &&) = delete;

    void init() {
        this->nWorking = 0;
        this->isStop = false;
        this->isDone = false;
    }

    // the destructor waits for all the functions in the queue to be finished
    ~ThreadPool() {
        this->stop(true);
    }

    // get the number of running threads in the pool
    size_t size() { return this->threads.size(); }

    std::thread &get_thread(size_t i) { return *this->threads[i]; }

    // change the number of threads in the pool
    // should be called from one thread, otherwise be careful to not interleave, also with this->stop()
    // nThreads must be >= 0
    void resize(size_t nThreads) {
        if (!this->isStop && !this->isDone) {
            size_t oldNThreads = this->threads.size();
            if (oldNThreads <= nThreads) {  // if the number of threads is increased
                this->nWorking.fetch_add(nThreads - oldNThreads);
                this->threads.resize(nThreads);
                this->flags.resize(nThreads);

                for (size_t i = oldNThreads; i < nThreads; ++i) {
                    this->flags[i] = std::make_shared<std::atomic<bool>>(false);
                    this->set_thread(i);
                }
            } else {  // the number of threads is decreased
                for (size_t i = oldNThreads - 1; i >= nThreads; --i) {
                    *this->flags[i] = true;  // this thread will finish
                    this->threads[i]->detach();
                }
                {
                    // stop the detached threads that were waiting
                    std::lock_guard<std::mutex> lock(this->mutex);
                    this->cv.notify_all();
                }
                this->threads.resize(nThreads);  // safe to delete because the threads are detached
                this->flags.resize(
                        nThreads);  // safe to delete because the threads have copies of shared_ptr of the flags, not originals
            }
        }
    }

    // empty the queue
    void clear_queue() {
        std::function<void(int id)> *_f;
        while (this->mTaskQueue.pop(_f))
            delete _f; // empty the queue
    }

    // pops a functional wrapper to the original function
    std::function<void(int)> pop() {
        std::function<void(int id)> *_f = nullptr;
        this->mTaskQueue.pop(_f);
        std::unique_ptr<std::function<void(int id)>>
                func(_f); // at return, delete the function even if an exception occurred
        std::function<void(int)> f;
        if (_f)
            f = *_f; // may exception ?
        return f;
    }

    // wait for all computing threads to finish and stop all threads
    // may be called asynchronously to not pause the calling thread while waiting
    // if isWait == true, all the functions in the queue are run, otherwise the queue is cleared without running the functions
    void stop(bool isWait = false) {
        if (!isWait) {
            if (this->isStop)
                return;
            this->isStop = true;
            for (size_t i = 0, n = this->size(); i < n; ++i) {
                *this->flags[i] = true;  // command the threads to stop
            }
            this->clear_queue();  // empty the queue
        } else {
            if (this->isDone || this->isStop)
                return;
            this->isDone = true;  // give the waiting threads a command to finish
        }
        {
            std::lock_guard<std::mutex> lock(this->mutex);
            this->cv.notify_one();  // stop all waiting threads
        }
        for (auto &thread : this->threads) {  // wait for the computing threads to finish
            if (thread->joinable())
                thread->join();
        }
        // if there were no threads in the pool but some functors in the queue, the functors are not deleted by the threads
        // therefore delete them here
        this->clear_queue();
        this->threads.clear();
        this->flags.clear();
    }

    template<typename F, typename... Rest>
    auto push(F &&f, Rest &&... rest) -> std::future<std::invoke_result_t<F, int, Rest...>> {
        auto pck = std::make_shared<std::packaged_task<std::invoke_result_t<F, int, Rest...>(int)>>(
                std::bind(std::forward<F>(f), std::placeholders::_1, std::forward<Rest>(rest)...)
        );
        auto _f = new std::function<void(int id)>([pck](int id) {
            (*pck)(id);
        });
        this->mTaskQueue.push(_f);
        std::lock_guard<std::mutex> lock(this->mutex);
        this->cv.notify_one();
        return pck->get_future();
    }

    // run the user's function that excepts argument int - id of the running thread. returned value is templatized
    // operator returns std::future, where the user can get the result and rethrow the catched exceptins
    template<typename F>
    auto push(F &&f) -> std::future<std::invoke_result_t<F, int>> {
        auto pck = std::make_shared<std::packaged_task<std::invoke_result_t<F, int>(int)>>(std::forward<F>(f));
        auto _f = new std::function<void(int id)>([pck](int id) {
            (*pck)(id);
        });
        this->mTaskQueue.push(_f);
        std::lock_guard<std::mutex> lock(this->mutex);
        this->cv.notify_one();
        return pck->get_future();
    }

private:

    void set_thread(int i) {
        auto f = [this, i, flag(this->flags[i])/* a copy of the shared ptr to the flag */]() {
            std::atomic<bool> &_flag = *flag;
            std::function<void(int id)> *_f;
            bool isPop = this->mTaskQueue.pop(_f);
            while (true) {
                while (isPop) {  // if there is anything in the queue
                    std::unique_ptr<std::function<void(int id)>>
                            func(_f); // at return, delete the function even if an exception occurred
                    (*_f)(i);
                    if (_flag)
                        return;  // the thread is wanted to stop, return even if the queue is not empty yet
                    else
                        isPop = this->mTaskQueue.pop(_f);
                }
                // the queue is empty here, wait for the next command
                std::unique_lock<std::mutex> lock(this->mutex);
                --this->nWorking;
                if (this->isDone && this->nWorking == 0) {
                    cv.notify_all();
                    return;
                }
                this->cv.wait(lock, [this, &_f, &isPop, &_flag]() {
                    isPop = this->mTaskQueue.pop(_f);
                    return isPop || (this->isDone && this->nWorking == 0) || _flag;
                });
                if (!isPop)
                    return;  // if the queue is empty and this->isDone == true or *flag then return
                ++this->nWorking;
            }
        };
        this->threads[i] = std::make_unique<std::thread>(f);
    }

    std::vector<std::unique_ptr<std::thread>> threads;
    std::vector<std::shared_ptr<std::atomic<bool>>> flags;
    LockQueue<std::function<void(int id)> *> mTaskQueue;
    std::atomic<bool> isDone{};
    std::atomic<bool> isStop{};
    std::atomic<size_t> nWorking{};  // how many threads are waiting

    std::mutex mutex;
    std::condition_variable cv;
};

}
#endif //DAGFLOW_COMMON_ThreadPool_H_
