# Lesson 2: 线程生命周期

### 学习目标
- 理解join()和detach()的区别
- 掌握线程的生命周期管理
- 学会处理线程异常

### 知识点
1. 线程的状态
   - 可结合的（Joinable）：新创建的线程
   - 已分离的（Detached）：调用detach()后的线程
   - 已完成的（Finished）：调用join()后的线程
   - 空线程：默认构造的线程

2. 线程管理方式
   - join()：等待线程完成
   - detach()：线程在后台运行
   - joinable()：检查线程状态

3. RAII线程管理
   - 自动管理线程生命周期
   - 确保线程正确join/detach
   - 处理异常情况
   - 移动语义在线程管理中的应用

4. 异常处理
   - 在线程函数中抛出异常
   - 使用try-catch捕获异常
   - 确保异常不影响线程管理
   - RAII确保资源正确释放

### 练习内容回顾
1. join和detach的使用
   - 理解两种方式的区别
   - 观察执行顺序的不同
   - 使用joinable()检查线程状态

2. RAII类的实现
   - 创建线程包装类
   - 使用移动语义
   - 自动管理线程生命周期

3. 异常处理实践
   - 实现异常安全的线程函数
   - 正确捕获和处理异常
   - 观察异常处理流程

### 注意事项
1. 线程必须在析构前被join或detach
2. detach后的线程不能再控制
3. 使用RAII避免资源泄露
4. 确保异常不会导致线程资源泄露
5. 注意线程状态的检查

### 下一课预告
- 互斥锁基础
- 保护共享资源
- 避免数据竞争 