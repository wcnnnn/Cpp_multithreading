#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>
#include <mutex>

int normal_counter = 0;
std::atomic<int> atomic_counter(0);
std::atomic<int> atomic_cas(0);
std::mutex mtx;

void increment_normal(){
    for (size_t i = 0; i < 10000000; i++)
    {
        std::lock_guard<std::mutex> lock(mtx);
        normal_counter++;
    }
    
}

void increment_cas(){
    for (size_t i = 0; i < 10000000; i++)
    {
        int expected = atomic_cas.load();
        while (!atomic_cas.compare_exchange_weak(expected,expected+1));
        {
        }
    }
}


void increment_atomic(){
    for (size_t i = 0; i < 10000000; i++)
    {
        atomic_counter.fetch_add(1,std::memory_order_seq_cst);
    }
    
}

// 顺序访问
void sequential_access(std::vector<int>& data) {
    for(size_t i = 0; i < data.size(); ++i) {
        data[i]++;
    }
}

// 跳跃访问
void strided_access(std::vector<int>& data) {
    // 每个步长都遍历一遍
    for(size_t stride = 0; stride < 16; ++stride) {
        for(size_t i = stride; i < data.size(); i += 16) {
            data[i]++;
        }
    }
}

void numa_aware_access(std::vector<int>& data,size_t threadid,size_t num_threads){
    size_t block_size = data.size() / num_threads;
    size_t start = threadid*block_size;
    size_t end = (threadid==num_threads-1) ? data.size() : (threadid+1) * block_size;
    for (size_t i = start; i < end; i++)
    {
        data[i]++;
    }
    
}

void normal_vector_add(const std::vector<double>& a, const std::vector<double>& b, std::vector<double>& c) {
    // 确保输入向量大小相同
    if (a.size() != b.size()) {
        throw std::invalid_argument("Vectors must be of the same size");
    }

    // 初始化结果向量
    c.resize(a.size());

    // 执行逐元素相加
    for (size_t i = 0; i < a.size(); ++i) {
        c[i] = a[i] + b[i];
    }
}

__attribute__((optimize("O3,tree-vectorize")))
void vectorize_add(const std::vector<double>& a, const std::vector<double>& b, std::vector<double>& c){
    // 确保输入向量大小相同
    if (a.size() != b.size()) {
        throw std::invalid_argument("Vectors must be of the same size");
    }

    // 初始化结果向量
    c.resize(a.size());

    // 执行逐元素相加
    for (size_t i = 0; i < a.size(); ++i) {
        c[i] = a[i] + b[i];
    }
}


int main() {
    std::vector<std::thread> threads;
    
    // 测试原子操作
    auto atomic_start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 4; ++i) {
        threads.emplace_back(increment_atomic);
    }
    for(auto& t : threads) {
        t.join();
    }
    auto atomic_end = std::chrono::high_resolution_clock::now();
    auto atomic_duration = std::chrono::duration_cast<std::chrono::milliseconds>(atomic_end - atomic_start);
    
    // 清空线程vector以便复用
    threads.clear();
    
    // 测试互斥锁
    auto mutex_start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 4; ++i) {
        threads.emplace_back(increment_normal);
    }
    for(auto& t : threads) {
        t.join();
    }
    auto mutex_end = std::chrono::high_resolution_clock::now();
    auto mutex_duration = std::chrono::duration_cast<std::chrono::milliseconds>(mutex_end - mutex_start);
    
    std::cout << "Atomic counter: " << atomic_counter 
              << " Time: " << atomic_duration.count() << "ms\n";
    std::cout << "Normal counter: " << normal_counter 
              << " Time: " << mutex_duration.count() << "ms\n";


    // 测试CAS操作
    auto cas_start = std::chrono::high_resolution_clock::now();
    threads.clear();
    for(int i = 0; i < 4; ++i) {
        threads.emplace_back(increment_cas);
    }
    for(auto& t : threads) {
        t.join();
    }
    auto cas_end = std::chrono::high_resolution_clock::now();
    auto cas_duration = std::chrono::duration_cast<std::chrono::milliseconds>(cas_end - cas_start);

    std::cout << "CAS counter: " << atomic_cas 
            << " Time: " << cas_duration.count() << "ms\n";

    // 创建测试数据
    const size_t data_size = 100000000;  // 1亿个元素
    std::vector<int> data(data_size, 1);

    // 测试顺序访问
    auto seq_start = std::chrono::high_resolution_clock::now();
    sequential_access(data);
    auto seq_end = std::chrono::high_resolution_clock::now();
    auto seq_duration = std::chrono::duration_cast<std::chrono::milliseconds>(seq_end - seq_start);

    // 测试跳跃访问
    auto str_start = std::chrono::high_resolution_clock::now();
    strided_access(data);
    auto str_end = std::chrono::high_resolution_clock::now();
    auto str_duration = std::chrono::duration_cast<std::chrono::milliseconds>(str_end - str_start);

    std::cout << "\nMemory Access Pattern Test:\n";
    std::cout << "Sequential access time: " << seq_duration.count() << "ms\n";
    std::cout << "Strided access time: " << str_duration.count() << "ms\n";

    // 测试NUMA感知访问
    threads.clear();
    data = std::vector<int>(data_size, 1);  // 重置数据
    
    auto numa_start = std::chrono::high_resolution_clock::now();
    for(size_t i = 0; i < 4; ++i) {
        threads.emplace_back(numa_aware_access, std::ref(data), i, 4);
    }
    for(auto& t : threads) {
        t.join();
    }
    auto numa_end = std::chrono::high_resolution_clock::now();
    auto numa_duration = std::chrono::duration_cast<std::chrono::milliseconds>(numa_end - numa_start);

    std::cout << "\nNUMA Test:\n";
    std::cout << "NUMA-aware access time: " << numa_duration.count() << "ms\n";

    // SIMD向量加法测试
    const size_t vector_size = 10000000;  // 1千万个元素
    std::vector<double> vec_a(vector_size, 1.0);
    std::vector<double> vec_b(vector_size, 2.0);
    std::vector<double> result_normal(vector_size);
    std::vector<double> result_simd(vector_size);

    // 测试普通向量加法
    auto normal_start = std::chrono::high_resolution_clock::now();
    normal_vector_add(vec_a, vec_b, result_normal);
    auto normal_end = std::chrono::high_resolution_clock::now();
    auto normal_duration = std::chrono::duration_cast<std::chrono::microseconds>(normal_end - normal_start);

    // 测试SIMD向量加法
    auto simd_start = std::chrono::high_resolution_clock::now();
    vectorize_add(vec_a, vec_b, result_simd);
    auto simd_end = std::chrono::high_resolution_clock::now();
    auto simd_duration = std::chrono::duration_cast<std::chrono::microseconds>(simd_end - simd_start);

    std::cout << "\nVector Addition Test:\n";
    std::cout << "Normal addition time: " << normal_duration.count() << "us\n";
    std::cout << "SIMD addition time: " << simd_duration.count() << "us\n";

    return 0;
} 