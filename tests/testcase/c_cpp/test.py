# coding=utf-8
import os
import _judger
import signal

from unittest import TestCase
from .. import base


class C_CPPJudgeTestCase(base.BaseTestCase):
    def setUp(self):
        self.workspace = self.init_workspace("c_cpp")
        self.config = {"max_cpu_time": 1000,
                       "max_real_time": 3000,
                       "max_memory": 1024 * 1024 * 1024,
                       "max_process_number": 10,
                       "max_output_size": 1024 * 1024,
                       "exe_path": "/bin/ls",
                       "input_path": "/dev/null",
                       "output_path": "/dev/null",
                       "error_path": "/dev/null",
                       "args": [],
                       "env": ["env=judger_test", "test=judger"],
                       "log_path": "judger_test.log",
                       "seccomp_rule_so_path": "/usr/lib/judger/librule_c_cpp.so",
                       "uid": 0,
                       "gid": 0}
        print "Running", self._testMethodName
