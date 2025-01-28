#include <thread>
#include <iostream>
#include <time.h>
#include <vector>

class count_machine
{
private:
    int count = 0;
public:
    count_machine(int initial = 0){
        count = initial;
    };

    void countadd(){
        count++;
    }

    int get_count(){
        return count;
    }
};

int main(){

    count_machine CM(0);
    std::vector<std::thread> threads;
    for (size_t i = 0; i < 5;i++)
    {
        threads.push_back(std::thread([&CM, i](){  // 捕获循环变量i
            CM.countadd();
            std::cout << "Thread " << i << " count: " << CM.get_count() << std::endl;
        }));
    }
    
    for (size_t i = 0; i < 5; i++)
    {
        threads[i].join();
    }

    return 0;
};


