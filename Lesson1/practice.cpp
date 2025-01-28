#include <thread>
#include <iostream>
#include <time.h>
#include <vector>
void giao(int n) {
    for (size_t i = 0; i < n; i++) {
        std::cout << "giao" << std::endl;
    }
}

void thread_id(int thread_id){
    std::cout << "Thread id: " << thread_id << " is running" << std::endl;
}


class practice
{
private:
    int n;
public:
    practice(int N){
        n = N;
    }

    void giao(int a) {
        for (size_t i = 0; i < a; i++) {
            std::cout << "giao" << std::endl;
    }
    }
};

int main() {
    // 创建多个线程
    std::thread thread_1(thread_id, 1); 
    std::thread thread_2(thread_id, 2);
    std::thread thread_3(thread_id, 3); 
    std::thread thread_4(thread_id, 4); 
    std::thread thread_5(thread_id, 5); 
    std::thread thread_6(thread_id, 6); 

    // 等待所有线程完成
    thread_1.join();
    thread_2.join();
    thread_3.join();
    thread_4.join();
    thread_5.join();
    thread_6.join();

    std::vector<std::thread> threads;
    for (size_t i = 0; i < 6; i++)
    {
        threads.push_back(std::thread(thread_id,i));
    }
    
    for (size_t i = 0; i < 6; i++)
    {
        threads[i].join();
    }

    practice pri(10);
    std::thread thread_lambda([&pri](){
        pri.giao(10);
    });

    thread_lambda.join();
    return 0;
} 