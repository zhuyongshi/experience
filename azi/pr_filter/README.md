# Pr_Filter方案 - 算法

## 编译&运行

### 预安装环境（linux环境）
1. 安装gcc和cmake环境（不赘述）
2. 安装CryptoPP密码库
    ```
    $ sudo apt-get install libcrypto++-dev libcrypto++-doc libcrypto++-utils

    $ apt-cache pkgnames | grep -i crypto++ 
    ```

### 编译命令
```
# 编译及运行
$ sh run.sh
# 编译可调试文件
$ sh run.sh debug
```