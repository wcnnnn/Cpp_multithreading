#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <chrono>
#include <map>

std::mutex mtx;
int counter = 0;
void count(){
    std::unique_lock<std::mutex> lock(mtx);
    counter++;
    lock.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    lock.lock();
    counter++;
}


class ThreadSafeCache
{
private:
    mutable std::shared_mutex rwmtx;
    std::map<int,std::string> cache;

public:
    std::string read(int key) const{
        std::shared_lock<std::shared_mutex> lock(rwmtx);
        auto it = cache.find(key);
        if (it!=cache.end())
        {
            return it->second;
        }else{
            return "not found";
        }
    };
    void write(int key,const std::string& value){
        std::unique_lock<std::shared_mutex> lock(rwmtx);
        cache[key] = value; 
    }

};

void reader(const ThreadSafeCache& cache, int key) {
    for(int i = 0; i < 5; i++) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::string value = cache.read(key);
        std::cout << std::ctime(&time) << "Reader " 
                  << std::this_thread::get_id() 
                  << " reads: " << value << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void writer(ThreadSafeCache& cache, int key, const std::string& value) {
        for(int i = 0; i < 3; i++) {
            cache.write(key, value + std::to_string(i));
            std::cout << "Thread " << std::this_thread::get_id() 
                    << " writes: " << value + std::to_string(i) << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
}


class BankAccount
{
private:
    mutable std::mutex mtx;
    double balance;
public:
    BankAccount(double initial_balance) : balance(initial_balance){}

    double getBalance() const{
        std::lock_guard<std::mutex> lock(mtx);
        return balance;
    }
    void transfer(BankAccount& other, double amount) {
        std::lock(mtx, other.mtx);
        std::lock_guard<std::mutex> lock1(mtx, std::adopt_lock);
        std::lock_guard<std::mutex> lock2(other.mtx, std::adopt_lock);
        
        if (balance >= amount) {
            balance -= amount;
            other.balance += amount;
        }
    }
};



int main() {
    std::vector<std::thread> thread1s;
    for (size_t i = 0; i < 4; i++)
    {
        thread1s.emplace_back(count);
    }
    
    for (auto & thread:thread1s)
    {
        thread.join();
    }
    std::cout << "Final counter value: " << counter << std::endl;

    ThreadSafeCache cache;
    cache.write(1, "initial_data");
    std::vector<std::thread> thread2s;
    for (size_t i = 0; i < 4; i++)
    {
        thread2s.emplace_back(reader,std::ref(cache),1);
    }
    
     // 创建写线程
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    thread2s.emplace_back(writer, std::ref(cache), 1, "data");
    
    // 等待所有线程完成
    for(auto& t : thread2s) {
        t.join();
    }

    // 测试银行账户转账
    BankAccount account1(1000);  // 账户1初始余额1000
    BankAccount account2(1000);  // 账户2初始余额1000

    std::cout << "\nTesting bank accounts..." << std::endl;
    std::cout << "Initial balances: " 
              << "Account1: " << account1.getBalance() 
              << " Account2: " << account2.getBalance() << std::endl;

    std::vector<std::thread> thread3s;
    // 创建多个线程进行转账
    for(int i = 0; i < 5; i++) {
        // 一些线程从account1转到account2
        thread3s.emplace_back([&account1, &account2]() {
            account1.transfer(account2, 100);
        });
        // 一些线程从account2转到account1
        thread3s.emplace_back([&account1, &account2]() {
            account2.transfer(account1, 100);
        });
    }

    // 等待所有转账完成
    for(auto& t : thread3s) {
        t.join();
    }

    std::cout << "Final balances: " 
              << "Account1: " << account1.getBalance() 
              << " Account2: " << account2.getBalance() << std::endl;

    return 0;
} 