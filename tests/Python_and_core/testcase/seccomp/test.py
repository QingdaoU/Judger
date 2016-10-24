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
                       "max_memory": 1024 * 1024 * 128,
                       "max_process_number": 10,
                       "max_output_size": 1024 * 1024,
                       "exe_path": "/bin/ls",
                       "input_path": "/dev/null",
                       "output_path": "/dev/null",
                       "error_path": "/dev/null",
                       "args": [],
                       "env": ["env=judger_test", "test=judger"],
                       "log_path": "judger_test.log",
                       "seccomp_rule_name": None,
                       "uid": 0,
                       "gid": 0}
        self.workspace = self.init_workspace("integration")

    def _compile_c(self, src_name, extra_flags=None):
        return super(SeccompTest, self)._compile_c("../../test_src/seccomp/" + src_name, extra_flags)

    def test_fork(self):
        config = self.config
        config["max_memory"] = 1024 * 1024 * 1024
        config["exe_path"] = self._compile_c("fork.c")
        config["output_path"] = config["error_path"] = self.output_path()
        result = _judger.run(**config)

        # without seccomp
        self.assertEqual(result["result"], _judger.RESULT_SUCCESS)

        # with general seccomp
        config["seccomp_rule_name"] = "general"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], 31)

        # with c_cpp seccomp
        config["seccomp_rule_name"] = "c_cpp"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], 31)

    def test_execve(self):
        config = self.config
        config["max_memory"] = 1024 * 1024 * 1024
        config["exe_path"] = self._compile_c("execve.c")
        config["output_path"] = config["error_path"] = self.output_path()
        result = _judger.run(**config)
        # without seccomp
        self.assertEqual(result["result"], _judger.RESULT_SUCCESS)
        self.assertEqual("Helloworld\n", self.output_content(config["output_path"]))

        # with general seccomp
        config["seccomp_rule_name"] = "general"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], 31)

        # with c_cpp seccomp
        config["seccomp_rule_name"] = "c_cpp"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], 31)

    def test_write_file(self):
        config = self.config
        config["max_memory"] = 1024 * 1024 * 1024
        config["exe_path"] = self._compile_c("write_file.c")
        config["output_path"] = config["error_path"] = self.output_path()
        result = _judger.run(**config)
        # without seccomp
        self.assertEqual(result["result"], _judger.RESULT_SUCCESS)
        self.assertEqual("test", self.output_content("/tmp/fffffffffffffile.txt"))

        # with general seccomp
        config["seccomp_rule_name"] = "general"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], 31)

        # with c_cpp seccomp
        config["seccomp_rule_name"] = "c_cpp"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], 31)
