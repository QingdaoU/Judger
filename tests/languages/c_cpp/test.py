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

    def _compile(self, src_name):
        path = os.path.dirname(os.path.abspath(__file__))
        exe_path = os.path.join(self.workspace, src_name.split(".")[0])
        cmd = "gcc {0} -o {1}".format(os.path.join(path, src_name), exe_path)
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

    def test_normal(self):
        config = self.config
        config["exe_path"] = self._compile("normal.c")
        config["input_path"] = self.make_input("judger_test")
        config["output_path"] = config["error_path"] = self.output_path()
        result = _judger.run(**config)
        output = """judger_test
Hello world"""
        self.assertEqual(result["result"], _judger.SUCCESS)
        self.assertEqual(output, self.output_content(config["output_path"]))

    def test_args(self):
        config = self.config
        config["exe_path"] = self._compile("args.c")
        config["args"] = ["test", "hehe", "000"]
        config["output_path"] = config["error_path"] = self.output_path()
        result = _judger.run(**config)
        output = """argv[0]: /tmp/c_cpp/args
argv[1]: test
argv[2]: hehe
argv[3]: 000
"""
        self.assertEqual(result["result"], _judger.SUCCESS)
        self.assertEqual(output, self.output_content(config["output_path"]))

    def test_env(self):
        config = self.config
        config["exe_path"] = self._compile("env.c")
        config["output_path"] = config["error_path"] = self.output_path()
        result = _judger.run(**config)
        output = """judger_test
judger
"""
        self.assertEqual(result["result"], _judger.SUCCESS)
        self.assertEqual(output, self.output_content(config["output_path"]))

    def test_real_time(self):
        config = self.config
        config["exe_path"] = self._compile("sleep.c")
        config["seccomp_rule_so_path"] = None
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.REAL_TIME_LIMIT_EXCEEDED)
        self.assertEqual(result["signal"], signal.SIGKILL)
        self.assertTrue(result["real_time"] >= config["max_real_time"])

    def test_cpu_time(self):
        config = self.config
        config["exe_path"] = self._compile("while1.c")
        config["seccomp_rule_so_path"] = None
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.CPU_TIME_LIMIT_EXCEEDED)
        self.assertEqual(result["signal"], signal.SIGKILL)
        self.assertTrue(result["cpu_time"] >= config["max_cpu_time"])

    def test_memory(self):
        config = self.config
        config["max_memory"] = 64 * 1024 * 1024
        config["exe_path"] = self._compile("memory1.c")
        result = _judger.run(**config)
        # malloc succeeded
        self.assertTrue(result["memory"] > 80 * 1024 * 1024)
        self.assertEqual(result["result"], _judger.MEMORY_LIMIT_EXCEEDED)

    def test_memory2(self):
        config = self.config
        config["max_memory"] = 64 * 1024 * 1024
        config["exe_path"] = self._compile("memory2.c")
        result = _judger.run(**config)
        # malloc failed, return 1
        self.assertEqual(result["exit_code"], 1)
        # malloc failed, so it should use a little memory
        self.assertTrue(result["memory"] < 12 * 1024 * 1024)
        self.assertEqual(result["result"], _judger.RUNTIME_ERROR)

    def test_re1(self):
        config = self.config
        config["exe_path"] = self._compile("re1.c")
        result = _judger.run(**config)
        # re1.c return 25
        self.assertEqual(result["exit_code"], 25)
