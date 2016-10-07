# coding=utf-8
import os
import shutil
from unittest import TestCase


class RunResult(object):
    cpu_time_limited = 1
    real_time_limit_exceeded = 2
    memory_limit_exceeded = 3
    runtime_error = 4
    system_error = 5


class BaseTestCase(TestCase):
    def init_workspace(self, language):
        base_workspace = "/tmp"
        workspace = os.path.join(base_workspace, language)
        shutil.rmtree(workspace, ignore_errors=True)
        os.system("mkdir -p " + workspace)
        return workspace

    def rand_str(self):
        return ''.join(map(lambda xx:(hex(ord(xx))[2:]), os.urandom(16)))

    def _compile_c(self, src_name, extra_flags=None):
        path = os.path.dirname(os.path.abspath(__file__))
        exe_path = os.path.join(self.workspace, src_name.split("/")[-1].split(".")[0])
        flags = " "
        if extra_flags:
            flags += " ".join(extra_flags)
        cmd = ("gcc {0} -g -O0 -static -o {1}" + flags).format(os.path.join(path, src_name), exe_path)
        if os.system(cmd):
            raise AssertionError("compile error, cmd: {0}".format(cmd))
        return exe_path

    def _compile_cpp(self, src_name):
        path = os.path.dirname(os.path.abspath(__file__))
        exe_path = os.path.join(self.workspace, src_name.split("/")[-1].split(".")[0])
        cmd = "g++ {0} -g -O0 -o {1}".format(os.path.join(path, src_name), exe_path)
        if os.system(cmd):
            raise AssertionError("compile error, cmd: {0}".format(cmd))
        return exe_path

    def make_input(self, content):
        path = os.path.join(self.workspace, self.rand_str())
        with open(path, "w") as f:
            f.write(content)
        return path

    def output_path(self):
        return os.path.join(self.workspace, self.rand_str())

    def output_content(self, path):
        with open(path, "r") as f:
            return f.read()