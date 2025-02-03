#include <thread>
#include <iostream>
#include <mutex>
#include <vector>

std::mutex mtx;
std::mutex mutex1;
std::mutex mutex2;

int counter = 0;

void increment(){
    for (size_t i = 0; i < 100000; i++)
    {
        std::lock_guard<std::mutex> lock(mtx);
        counter++;
       
    }
    
}

void threadA() {
    std::scoped_lock lock(mutex1, mutex2);  // 使用scoped_lock替代手动加锁
    std::cout << "线程A获得了所有锁" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // 执行一些操作...
}   // 作用域结束时自动解锁

void threadB() {
    std::scoped_lock lock(mutex1, mutex2);  // 使用scoped_lock替代手动加锁
    std::cout << "线程B获得了所有锁" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // 执行一些操作...
}   // 作用域结束时自动解锁

int main() {
    std::thread thread1(threadA);
    std::thread thread2(threadB);



    thread1.join();
    thread2.join();
    //std::cout<<"counter value"<<counter<<std::endl;
    return 0;
} 