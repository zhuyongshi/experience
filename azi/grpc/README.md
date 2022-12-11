# Pr_Filter方案 - GRPC交互

## 编译&运行

### 预安装环境（linux环境）
1. 安装grpc环境
    ```
    $ git clone https://github.com/grpc/grpc.git 

    $ cd grpc && git submodule update --init

    $ mkdir build && cd build

    $ cmake ..

    $ make -j

    $ sudo make install
    ```

### 编译命令
