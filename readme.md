# C++ 并行计算与高性能编程学习

## 学习目标
- 掌握C++多线程编程基础
- 理解并行计算核心概念
- 为CUDA/GPU编程打下基础

## 课程体系

| 课程 | 主要内容 | 实践代码 | 学习笔记 |
|------|----------|----------|----------|
| Lesson 1: 线程基础入门 | - 线程的创建与管理<br>- 参数传递机制<br>- 基本线程同步 | [practice.cpp](Lesson1/practice.cpp) | [note.md](Lesson1/note.md) |
| Lesson 2: 线程生命周期 | - 线程状态管理<br>- RAII设计模式<br>- 异常安全处理 | [practice.cpp](Lesson2/practice.cpp) | [note.md](Lesson2/note.md) |
| Lesson 3: 互斥锁与并行计算基础 | - 数据竞争问题<br>- 并行计算原理<br>- 并行矩阵乘法 | [practice.cpp](Lesson3/practice.cpp)<br>[matrix_multiply.cpp](Lesson3/matrix_multiply.cpp) | [note.md](Lesson3/note.md) |
| Lesson 4: 数据共享和保护 | - 读写锁机制<br>- 多锁管理策略<br>- 线程安全缓存系统 | [practice.cpp](Lesson4/practice.cpp) | [note.md](Lesson4/note.md) |
| Lesson 5: 条件变量与并行任务调度 | - 并行计算中的同步机制<br>- 任务调度与协调<br>- 并行任务调度系统 | [practice.cpp](Lesson5/practice.cpp) | [note.md](Lesson5/note.md) |
| Lesson 6: 原子操作与高性能计算 | - 原子操作应用<br>- 内存访问优化<br>- 高性能计算优化 | [practice.cpp](Lesson6/practice.cpp) | [note.md](Lesson6/note.md) |

## 主要实践项目
1. 并行矩阵乘法（Lesson 3）
2. 线程安全缓存系统（Lesson 4）
3. 并行任务调度器（Lesson 5）
4. 高性能向量计算（Lesson 6）

## 参考资源
1. C++并发编程实战
2. 并行程序设计导论
3. 高性能计算文档 