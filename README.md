# C++ Web Server

## 测试

3.18 实现epoll和fork多进程HTTP服务器，并测试短连接QPS

空闲状态
CPU:2\~3%
内存: 280\~290MB

| 服务器 | 短连接QPS | CPU峰值 | 内存峰值 |
| ------- | ------- | ------- | ------- |
| epoll IO复用服务器 | 6914 | 21.73% | 694.76MB |
| 多进程服务器 | 4304 | 92.32% | 298.07MB |

多进程服务器上下文切换频繁，QPS表现不佳。
