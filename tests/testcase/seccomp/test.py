# coding=utf-8
import _judger
import signal
import os

from .. import base


class SeccompTest(base.BaseTestCase):
    def setUp(self):
        print "Running", self._testMethodName
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
                       "seccomp_rule_so_path": None,
                       "uid": 0,
                       "gid": 0}
        self.workspace = self.init_workspace("seccomp")

    def _compile_c(self, src_name):
        return super(SeccompTest, self)._compile_c("seccomp/" + src_name)

    def test_mmap_write_file(self):
        config = self.config
        config["exe_path"] = self._compile_c("mmap.c")
        config["seccomp_rule_so_path"] = "/usr/lib/judger/librule_c_cpp.so"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], 31)
