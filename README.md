
#Judger [![Build Status](https://travis-ci.org/QingdaoU/Judger.svg?branch=master)](https://travis-ci.org/QingdaoU/Judger)


OnlineJudge 判题沙箱

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
 - 如果使用了 `use_nobody = True` 则需要 `root` 权限启动。
 - Python api 请不要使用中文 Unicode。
 
##感谢
 - https://github.com/lodevil/Lo-runner
 - https://github.com/quark-zju/lrun
 - https://github.com/daveho/EasySandbox