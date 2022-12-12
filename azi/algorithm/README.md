# Pr_Filter方案 - 算法

## 编译&运行

### 安装环境（linux环境）
1. 安装gcc环境（不赘述）
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

## 说明

| 文件             | 文件内容                                                     |
| ---------------- | ------------------------------------------------------------ |
| ecsse_util       | 基础密码工具类（包括aes、hash算法）                          |
| pr_filter_util   | pr_filter方案的基础函数（包括Permutation、Find_CK、Permutationkey_Gen、AONTH等） |
| pr_filter_server | pr_filter方案中server端需要函数（search）                    |
| pr_filter_client | pr_filter方案中client端需要函数（setup、token、resolve）     |