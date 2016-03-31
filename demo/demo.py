# coding=utf-8
import os
import judger

base_path = os.path.dirname(os.path.abspath(__file__))


def _compile():
    return judger.run(path="/usr/bin/gcc",
                      in_file=os.path.join(base_path, "in"),
                      out_file=os.path.join(base_path, "gcc_out"),
                      max_cpu_time=judger.CPU_TIME_UNLIMITED,
                      max_memory=judger.MEMORY_UNLIMITED,
                      args=[os.path.join(base_path, "demo.c"), "-o", os.path.join(base_path, "demo")],
                      env=["PATH=" + os.environ["PATH"]],
                      use_sandbox=False, 
                      use_nobody=False)


def run(use_sandbox, use_nobody):
    print "compile result: ", _compile()
    path = os.path.join(base_path, "demo")
    return judger.run(path=path,
                      in_file=os.path.join(base_path, "in"),
                      out_file=os.path.join(base_path, "out"),
                      # ms
                      max_cpu_time=2000,
                      # Byte
                      max_memory=200000000,
                      # args and env are optional
                      args=["1", "2", "####"],
                      env=["aaa=123"],
                      # default is True
                      use_sandbox=use_sandbox,
                      use_nobody=use_nobody)


print "sandbox and nobody"
print run(use_sandbox=True, use_nobody=True)

print '\n\nno sandbox and root'
print run(use_sandbox=False, use_nobody=False)
print "\n\nout: ", open("out").read()

print "\n\nno sandbox and nobody"
print run(use_sandbox=False, use_nobody=True)
print "\n\nout: ", open("out").read()
