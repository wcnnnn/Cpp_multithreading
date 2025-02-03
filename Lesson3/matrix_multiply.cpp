#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

class Matrix {
private:
    std::vector<std::vector<double>> data;
    size_t rows;
    size_t cols;

public:
    Matrix(size_t r, size_t c){
        rows = r;
        cols = c;
        data.resize(rows,std::vector<double> (cols,0.0));
    };
    void randomize(){
        for (size_t i = 0; i < rows; i++)
        {
            for (size_t j = 0; j < cols; j++)
            {
                data[i][j] = rand()%100;
            }
        }
    };  // 随机填充数据
    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }
    void print() const{
        for (size_t i = 0; i < rows; i++)
        {
            for (size_t j = 0; j < cols; j++)
            {
                std::cout<<data[i][j]<<std::endl;
            }
        }
    };
    
    // 访问器
    const std::vector<double>& operator[](size_t i) const { return data[i]; }
    std::vector<double>& operator[](size_t i) { return data[i]; }
};

// 并行矩阵乘法计算器
class ParallelMatrixMultiplier {
private:
    std::mutex mtx;
    const Matrix& A;
    const Matrix& B;
    Matrix& result;
    
public:
    ParallelMatrixMultiplier(const Matrix& a, const Matrix& b, Matrix& res):A(a),B(b),result(res){
        if (A.getCols() != B.getRows()) {
        throw std::invalid_argument("Matrix dimensions do not match for multiplication");
        }
        if (result.getRows() != A.getRows() || result.getCols() != B.getCols()) {
            throw std::invalid_argument("Result matrix has wrong dimensions");
        }
    };
    void multiplyRange(size_t start_row, size_t end_row){
        // 先在本地计算结果
        std::vector<std::vector<double>> local_result(end_row - start_row, 
            std::vector<double>(B.getCols()));
        
        // 无需加锁的计算阶段
        for (size_t i = 0; i < end_row - start_row; i++) {
            for (size_t j = 0; j < B.getCols(); j++) {
                double sum = 0.0;
                for (size_t k = 0; k < A.getCols(); k++) {
                    sum += A[i + start_row][k] * B[k][j];
                }
                local_result[i][j] = sum;
            }
        }
        
        // 只在写入最终结果时加锁
        {
            std::lock_guard<std::mutex> lock(mtx);
            for (size_t i = 0; i < end_row - start_row; i++) {
                for (size_t j = 0; j < B.getCols(); j++) {
                    result[i + start_row][j] = local_result[i][j];
                }
            }
        }
    } // 计算指定行范围

    // 添加单线程版本的矩阵乘法函数
    void multiplySequential() {
        for(size_t i = 0; i < A.getRows(); ++i) {
            for(size_t j = 0; j < B.getCols(); ++j) {
                double sum = 0;
                for(size_t k = 0; k < A.getCols(); ++k) {
                    sum += A[i][k] * B[k][j];
                }
                result[i][j] = sum;
            }
        }
    }

    void multiply(size_t num_threads){
        std::vector<std::thread> threads;
        size_t rows_per_threads = A.getRows() / num_threads;
        for (size_t i = 0; i < num_threads; i++)
        {
            size_t start_row = i*rows_per_threads;
            size_t end_row = (i==num_threads-1)?A.getRows():(i+1)*rows_per_threads;
            threads.emplace_back(&ParallelMatrixMultiplier::multiplyRange,this,start_row,end_row);
        }
        for (auto& thread:threads)
        {
            thread.join();
        }
        
    };  // 启动多线程计算
}; 

int main() {
    // 创建更大的测试矩阵
    const size_t size = 1000;  // 改为1000x1000
    Matrix A(size, size);
    Matrix B(size, size);
    Matrix C(size, size);  // 结果矩阵

    // 随机初始化矩阵
    A.randomize();
    B.randomize();

    // 创建乘法器
    ParallelMatrixMultiplier multiplier(A, B, C);

    // 测试单线程性能
    auto start = std::chrono::high_resolution_clock::now();
    multiplier.multiplySequential();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Sequential multiplication took: " << duration.count() << " ms" << std::endl;

    // 测试不同线程数的性能
    for(size_t num_threads = 1; num_threads <= 8; num_threads *= 2) {
        start = std::chrono::high_resolution_clock::now();
        multiplier.multiply(num_threads);
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << num_threads << " threads took: " << duration.count() << " ms" << std::endl;
    }

    return 0;
}