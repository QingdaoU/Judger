#Judger [developing]

Judger for OnlineJudge 

## build

```
sudo apt-get install libseccomp-dev
mkdir build && cd build && cmake .. && make && sudo make install
```


## C API

`#include <runner.h>` then call `run` function with `struct config` and `struct result` pointer.

## `struct config` members
 - `max_cpu_time`(ms):  max cpu time this process can cost, -1 for unlimited
 - `max_real_time`(ms):  max time this process can run, -1 for unlimited
 - `max_memory`(byte):  max size of the process's virtual memory (address space), -1 for unlimited
 - `max_process_number`:  max number of child process this process can create (to avoid fork bomb), -1 for unlimited
 - `max_output_size`(byte):  max size of data this process can output to stdout, stderr and file, -1 for unlimited
 - `exe_path`:  path of file to run
 - `input_file`:  redirect content of this file to process's stdin
 - `output_file`:  redirect process's stdout to this file
 - `error_file`:  redirect process's stderr to this file
 - `args`(string array terminated by NULL):  arguments to run this process
 - `env`(string array terminated by NULL):  environment variables this process can get
 - `log_path`:  judger log path
 - `seccomp_rule_so_path`(string or NULL): seccomp rules used to limit process system calls
 - `uid`:  user to run this process
 - `gid`:  user group this process belongs to
 
## `struct result` members

 - `cpu_time`:  cpu time the process has used
 - `real_time`:  actual running time of the process
 - `signal`:  signal number
 - `exit_code`:  process's exit code
 - `result`:  judger result, details in `runner.h`
 - `error`:  args validation error or judger internal error, error code in `runner.h`
 
## Python binding (Python 2.7 only)

```
sudo python setup.py install
```

### Python demo

```
>>> import _judger
>>> _judger.run(max_cpu_time=1000,
...             max_real_time=2000,
...             max_memory=1000000000,
...             max_process_number=200,
...             max_output_size=10000,
...             # five args above can be _judger.UNLIMITED
...             exe_path="/bin/echo",
...             input_path="/dev/null",
...             output_path="echo.out",
...             error_path="echo.out",
...             # can be empty list
...             args=["HelloWorld"],
...             # can be empty list
...             env=["foo=bar"],
...             log_path="judger.log",
...             # can be None
...             seccomp_rule_so_path="/usr/lib/judger/librule_c_cpp.so",
...             uid=0,
...             gid=0)

{'cpu_time': 0, 'signal': 0, 'memory': 4554752, 'exit_code': 0, 'result': 0, 'error': 0, 'real_time': 2}
```


## Run tests

```
cd tests &&  sudo python test.py
```

## Note

 - Linux x64 and kernel version > 3.17 required
 - Judger security relies on Docker with default security config [More](todo)
 - Tested under Ubuntu 14.04 docker container. System calls may vary due to different system and kernel versions
 - Root user required to run judger
 - Why use seccomp instead of ptrace? Ptrace can decrease process's performance significantly, for each system call, twice 
 context switch between child process and parent process is needed.
 - How to custom seccomp rule? [Example here]()  and then reinstall jduger, your code will be compiled and installed under 
 `/usr/lib/judger`
 
## License

  The Star And Thank Author License (SATA)