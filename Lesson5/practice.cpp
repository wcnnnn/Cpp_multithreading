#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <functional>

std::mutex mtx;
std::condition_variable cv;
bool condition = false;

void waitingThread() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, []{ return condition; });
    std::cout << "Condition is true, continuing execution." << std::endl;
}

void notifyingThread() {
    std::this_thread::sleep_for(std::chrono::seconds(2)); // 模拟一些工作
    {
        std::lock_guard<std::mutex> lock(mtx);
        condition = true;
        cv.notify_one();
    }
    std::cout << "Notified the waiting thread." << std::endl;
}

class TaskScheduler
{
private:
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::vector<std::thread> workers;
    std::condition_variable task_cv;
    bool stop_flag;

    void workerfunction() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                task_cv.wait(lock, [this] {
                    return !tasks.empty() || stop_flag;
                });
                
                if (stop_flag && tasks.empty()) {
                    return;
                }
                
                task = std::move(tasks.front());
                tasks.pop();
            }
            task();  // 执行任务
        }
    }
public:
    TaskScheduler(size_t nums_threads):stop_flag(false){
        workers.reserve(nums_threads);
    };

    void addTask(std::function<void()> task){
        {
           std::lock_guard<std::mutex> lock(queue_mutex);
            tasks.push(task); 
        }
        task_cv.notify_one();
    };

    void start() {
        for (size_t i = 0; i < workers.capacity(); ++i) {
            workers.emplace_back(&TaskScheduler::workerfunction, this);
        }
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            stop_flag = true;
        }
        task_cv.notify_all();  // 通知所有等待的线程
        
        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        workers.clear();
    }

    ~TaskScheduler(){
        stop();
    };
};


int main() {
    // 创建一个有4个工作线程的调度器
    TaskScheduler scheduler(4);
    scheduler.start();

    // 添加一些测试任务
    for (int i = 0; i < 8; ++i) {
        scheduler.addTask([i] {
            std::cout << "Task " << i << " started by thread " 
                      << std::this_thread::get_id() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 模拟工作
            std::cout << "Task " << i << " completed" << std::endl;
        });
    }

    // 等待一段时间让任务执行
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 停止调度器
    scheduler.stop();

    return 0;
}