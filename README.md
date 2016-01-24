
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

##注意
 - 因为需要切换为 `nobody` 用户运行用户代码，所以需要 `root` 权限启动。
 - Python api 请不要使用中文 Unicode。