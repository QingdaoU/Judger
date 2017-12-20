# coding=utf-8
from __future__ import print_function
import _judger
import signal
import shutil
import os

from .. import base


class SeccompTest(base.BaseTestCase):
    def setUp(self):
        print("Running", self._testMethodName)
        self.workspace = self.init_workspace("integration")

    def _compile_c(self, src_name, extra_flags=None):
        return super(SeccompTest, self)._compile_c("../../test_src/seccomp/" + src_name, extra_flags)

    def test_fork(self):
        config = self.base_config
        config["exe_path"] = self._compile_c("fork.c")
        config["output_path"] = config["error_path"] = self.output_path()
        result = _judger.run(**config)

        # without seccomp
        self.assertEqual(result["result"], _judger.RESULT_SUCCESS)

        # with general seccomp
        config["seccomp_rule_name"] = "general"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], self.BAD_SYSTEM_CALL)

        # with c_cpp seccomp
        config["seccomp_rule_name"] = "c_cpp"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], self.BAD_SYSTEM_CALL)

    def test_execve(self):
        config = self.base_config
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
        self.assertEqual(result["signal"], self.BAD_SYSTEM_CALL)

        # with c_cpp seccomp
        config["seccomp_rule_name"] = "c_cpp"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], self.BAD_SYSTEM_CALL)

    def test_write_file_using_open(self):
        config = self.base_config
        config["exe_path"] = self._compile_c("write_file_open.c")
        config["output_path"] = config["error_path"] = self.output_path()
        path = os.path.join(self.workspace, "file1.txt")
        config["args"] = [path, "w"]
        result = _judger.run(**config)
        # without seccomp
        self.assertEqual(result["result"], _judger.RESULT_SUCCESS)
        self.assertEqual("", self.output_content(path))

        # with general seccomp
        config["seccomp_rule_name"] = "general"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], self.BAD_SYSTEM_CALL)

        # with c_cpp seccomp
        config["seccomp_rule_name"] = "c_cpp"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], self.BAD_SYSTEM_CALL)

    def test_read_write_file_using_open(self):
        config = self.base_config
        config["exe_path"] = self._compile_c("write_file_open.c")
        config["output_path"] = config["error_path"] = self.output_path()
        path = os.path.join(self.workspace, "file2.txt")
        config["args"] = [path, "w+"]
        result = _judger.run(**config)
        # without seccomp
        self.assertEqual(result["result"], _judger.RESULT_SUCCESS)
        self.assertEqual("", self.output_content(path))

        # with general seccomp
        config["seccomp_rule_name"] = "general"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], self.BAD_SYSTEM_CALL)

        # with c_cpp seccomp
        config["seccomp_rule_name"] = "c_cpp"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], self.BAD_SYSTEM_CALL)

    def test_write_file_using_openat(self):
        config = self.base_config
        config["exe_path"] = self._compile_c("write_file_openat.c")
        config["output_path"] = config["error_path"] = self.output_path()
        path = os.path.join(self.workspace, "file3.txt")
        config["args"] = [path, "w"]
        result = _judger.run(**config)
        # without seccomp
        self.assertEqual(result["result"], _judger.RESULT_SUCCESS)
        self.assertEqual("", self.output_content(path))

        # with general seccomp
        config["seccomp_rule_name"] = "general"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], self.BAD_SYSTEM_CALL)

        # with c_cpp seccomp
        config["seccomp_rule_name"] = "c_cpp"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], self.BAD_SYSTEM_CALL)

    def test_read_write_file_using_openat(self):
        config = self.base_config
        config["exe_path"] = self._compile_c("write_file_openat.c")
        config["output_path"] = config["error_path"] = self.output_path()
        path = os.path.join(self.workspace, "file4.txt")
        config["args"] = [path, "w+"]
        result = _judger.run(**config)
        # without seccomp
        self.assertEqual(result["result"], _judger.RESULT_SUCCESS)
        self.assertEqual("", self.output_content(path))

        # with general seccomp
        config["seccomp_rule_name"] = "general"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], self.BAD_SYSTEM_CALL)

        # with c_cpp seccomp
        config["seccomp_rule_name"] = "c_cpp"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], self.BAD_SYSTEM_CALL)

    def test_sysinfo(self):
        config = self.base_config
        config["exe_path"] = self._compile_c("sysinfo.c")
        result = _judger.run(**config)

        self.assertEqual(result["result"], _judger.RESULT_SUCCESS)

    def test_exceveat(self):
        config = self.base_config
        config["exe_path"] = self._compile_c("execveat.c")
        config["output_path"] = config["error_path"] = self.output_path()
        result = _judger.run(**config)
        if "syscall not found" in self.output_content(config["output_path"]):
            print("execveat syscall not found, test ignored")
            return
        self.assertEqual(result["result"], _judger.RESULT_SUCCESS)
        
        # with general seccomp 
        config["seccomp_rule_name"] = "general"
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], self.BAD_SYSTEM_CALL)
