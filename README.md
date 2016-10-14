
#Judger [![Build Status](https://travis-ci.org/QingdaoU/Judger.svg?branch=master)](https://travis-ci.org/QingdaoU/Judger)


OnlineJudge 判题沙箱

本项目属于历史项目，最新代码请看`newnew`分支，相关文档请看 http://docs.onlinejudge.me

##Python api 使用方法

见 demo 

```
sudo python setup.py install
cd demo && sudo python demo.py
```

##C语言 api 使用方法

`#include "runner.c"`即可，然后传入 `config` 和 `result` 的结构体指针。


## 为什么不是ptrace

ptrace在很多OJ上都有应用，但是不可否认的是ptrace存在一个重大缺点：严重影响进程运行的性能，因为每次系统调用就要进行两次上下文切换，从子进程到父进程，然后父进程到子进程。OJ上题目很多都需要大量的输入和输出，会产生大量的系统调用，导致代码运行时间加长。

##注意
 - 本Judger定位于C/C++语言，如果使用Java，由于JVM的特殊性，请设置`use_sandbox=False， max_memory=MEMORY_UNLIMITED`，安全方面使用Java自带的安全策略，内存限制方面使用`Xms`、`Xmx`等参数。参考[这里](https://github.com/QingdaoU/OnlineJudge/blob/master/judge)关于Java的部分注释。
 - `runner.c`里面硬编码了系统调用白名单，在Ubuntu 14.04 64位系统上测试通过。如果在您的系统上正常程序出现了`Runtime Error`可能是部分系统调用不一致导致的。如果怀疑是这个原因提出issue，请务必提供系统版本和`strace ./FILE_NAME`的结果。目前已知32位系统肯定会出现非白名单系统调用，但是因为32位系统无法使用docker，一般出现在本地测试环境中。
 - 如果使用了 `use_nobody = True` 则需要 `root` 权限启动。
 - Python api 请只使用str。
 
##感谢
 - https://github.com/lodevil/Lo-runner
 - https://github.com/quark-zju/lrun
 - https://github.com/daveho/EasySandbox
