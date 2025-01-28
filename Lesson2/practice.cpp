#include <thread>
#include <iostream>
#include <vector>
#include <chrono>

void print_with_sleep(){
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "111" << std::endl;
}

void thread_with_exception() {
    std::cout << "\n=== Inside thread_with_exception ===" << std::endl;
    std::cout << "Thread starting..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "About to throw exception..." << std::endl;
    throw std::runtime_error("Exception in thread!");  // 抛出异常
}

void error_test() {
    std::cout << "\n=== Inside error_test ===" << std::endl;
    std::cout << "Starting error_test..." << std::endl;
    try {
        std::cout << "Before calling thread_with_exception" << std::endl;
        thread_with_exception();
        std::cout << "This line should not be printed" << std::endl;  // 因为上面会抛出异常
    } catch(const std::exception& e) {
        std::cout << "=== Exception caught ===" << std::endl;
        std::cerr << "Exception message: " << e.what() << std::endl;
    }
    std::cout << "error_test finished" << std::endl;
}

class RAII
{
private:
    std::thread thread1;
    bool use_join;
public:
//移动语义
RAII(std::thread thread,bool join = true):thread1(std::move(thread)),use_join(join)
{
}

~RAII()
{
    if (thread1.joinable())
    {
        if (use_join)
        {
            thread1.join();
        }else{
            thread1.detach();
        } 
    } 
}
};

int main() {
    // 1. 检查join后的线程
    std::thread t1(print_with_sleep);
    std::cout << "t1 joinable before join: " << std::boolalpha << t1.joinable() << std::endl;  // 使用std::boolalpha输出true/false
    t1.join();
    std::cout << "t1 joinable after join: " << t1.joinable() << std::endl;

    // 2. 检查detach后的线程
    std::thread t2(print_with_sleep);
    std::cout << "t2 joinable before detach: " << t2.joinable() << std::endl;
    t2.detach();
    std::cout << "t2 joinable after detach: " << t2.joinable() << std::endl;

    // 3. 检查空线程
    std::thread t3;
    std::cout << "Empty thread joinable: " << t3.joinable() << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(3));

    // 测试 RAII 类
    {  // join 测试
        RAII thread_guard1(std::thread(print_with_sleep));  // 使用不同的名字
        std::cout << "Thread created with join" << std::endl;
    }  // 这里自动调用析构函数，执行 join

    {  // detach 测试
        RAII thread_guard2(std::thread(print_with_sleep), false);  // 使用不同的名字
        std::cout << "Thread created with detach" << std::endl;
    }  // 这里自动调用析构函数，执行 detach
    std::this_thread::sleep_for(std::chrono::seconds(3));  // 在这里等待 detach 的线程
    
    {
    std::thread empty_thread;  // 创建空线程
    RAII guard(std::move(empty_thread));
    std::cout << "Using empty thread" << std::endl;

    }
    std::cout << "\n=== Direct Exception Test ===" << std::endl;
    error_test();  // 直接调用，不使用线程
    {
        RAII thread_guard3(std::thread(error_test));
        std::cout << "Thread with exception handling created" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));  // 给线程足够的时间执行
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 0;
}