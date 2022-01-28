#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <vector>
#include <atomic>

class ThreadPool
{
public:
    ThreadPool(unsigned threadNumber = std::thread::hardware_concurrency())
    {
        while (threadNumber > 0)
        {
            threads_.push_back(std::async(std::launch::async, &ThreadPool::work, this));
            --threadNumber;
        }
    }

    ~ThreadPool()
    {
        stop();
    }

    void addTask(std::function<double()> task)
    {
        {
            const auto lock = std::lock_guard{tasksMutex_};
            tasks_.push_back(std::move(task));
        }
        workConditionVariable_.notify_one();
    }

    double average()
    {
        const auto lock = std::lock_guard{endTaskMutex_};
        return average_ / endedTaskCounter_;
    }

    void stop()
    {
        {
            auto lock = std::lock_guard{tasksMutex_};
            isStopRequested_ = true;
        }
        workConditionVariable_.notify_one();
        for(auto& future : threads_)
        {
            future.wait();
        }
    }

private:

    void work()
    {
        std::this_thread::sleep_for(std::chrono::seconds(1U));
        while(true)
        {
            auto lock = std::unique_lock{tasksMutex_};
            workConditionVariable_.wait(lock, [this](){ return !std::empty(tasks_) || isStopRequested_; });

            if (!std::empty(tasks_))
            {
                auto task = std::move(tasks_.back());
                tasks_.pop_back();
                lock.unlock();
                const auto result = task();

                const auto endLock = std::lock_guard{endTaskMutex_};
                average_ += result;
                ++endedTaskCounter_;
            }
            else if (isStopRequested_)
            {
                std::cout << "quit " << std::this_thread::get_id() << std::endl;
                lock.unlock();
                workConditionVariable_.notify_one();
                return;
            }
        }
    }

    std::vector<std::function<double()>> tasks_;
    std::vector<std::future<void>> threads_;
    std::mutex tasksMutex_;
    std::mutex endTaskMutex_;
    std::condition_variable workConditionVariable_;
    double average_{0.0};
    unsigned endedTaskCounter_{0U};
    bool isStopRequested_{false};
};

double calculate()
{
    static auto i = unsigned{0};
    std::this_thread::sleep_for(std::chrono::seconds(++i));
    return 1;
}

int main(int argc, char *argv[])
{
    auto pool = ThreadPool{11};
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "adding" << std::endl;
    for(int i = 0; i < 10; ++i)
    {
        pool.addTask(calculate);
    }
    std::cout << "added" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5U));
    pool.stop();
    std::cout << pool.average() << std::endl;
}
