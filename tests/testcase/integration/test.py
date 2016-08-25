# coding=utf-8
import _judger
import signal
import pwd
import grp

from .. import base


class IntegrationTest(base.BaseTestCase):
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
                       "seccomp_rule_so_path": "/usr/lib/judger/librule_c_cpp.so",
                       "uid": 0,
                       "gid": 0}
        self.workspace = self.init_workspace("integration")
        
    def _compile(self, src_name):
        return super(IntegrationTest, self)._compile("integration/" + src_name)

    def test_args_validation(self):
        with self.assertRaisesRegexp(ValueError, "Invalid args and kwargs"):
            _judger.run()
            _judger.run(a=1, c=3)

    def test_args_must_be_list(self):
        with self.assertRaisesRegexp(ValueError, "args must be a list"):
            _judger.run(max_cpu_time=1000, max_real_time=2000, max_memory=1000000000,
                        max_process_number=200, max_output_size=10000, exe_path="1.out",
                        input_path="1.in", output_path="1.out", error_path="1.out",
                        args="12344", env=["a=b"], log_path="1.log",
                        seccomp_rule_so_path="1.so", uid=0, gid=0)

        with self.assertRaisesRegexp(ValueError, "args must be a list"):
            _judger.run(max_cpu_time=1000, max_real_time=2000, max_memory=1000000000,
                        max_process_number=200, max_output_size=10000, exe_path="1.out",
                        input_path="1.in", output_path="1.out", error_path="1.out",
                        args={"k": "v"}, env=["a=b"], log_path="1.log",
                        seccomp_rule_so_path="1.so", uid=0, gid=0)

    def test_args_item_must_be_string(self):
        with self.assertRaisesRegexp(ValueError, "arg item must be a string"):
            _judger.run(max_cpu_time=1000, max_real_time=2000, max_memory=1000000000,
                        max_process_number=200, max_output_size=10000, exe_path="1.out",
                        input_path="1.in", output_path="1.out", error_path="1.out",
                        args=["1234", 1234], env=["a=b"], log_path="1.log",
                        seccomp_rule_so_path="1.so", uid=0, gid=0)

        with self.assertRaisesRegexp(ValueError, "arg item must be a string"):
            _judger.run(max_cpu_time=1000, max_real_time=2000, max_memory=1000000000,
                        max_process_number=200, max_output_size=10000, exe_path="1.out",
                        input_path="1.in", output_path="1.out", error_path="1.out",
                        args=["1234", None], env=["a=b"], log_path="1.log",
                        seccomp_rule_so_path="1.so", uid=0, gid=0)

        with self.assertRaisesRegexp(ValueError, "arg item must be a string"):
            _judger.run(max_cpu_time=1000, max_real_time=2000, max_memory=1000000000,
                        max_process_number=200, max_output_size=10000, exe_path="1.out",
                        input_path="1.in", output_path="1.out", error_path="1.out",
                        args=[u"哈哈哈"], env=["a=b"], log_path="1.log",
                        seccomp_rule_so_path="1.so", uid=0, gid=0)

    def test_env_must_be_list(self):
        with self.assertRaisesRegexp(ValueError, "env must be a list"):
            _judger.run(max_cpu_time=1000, max_real_time=2000, max_memory=1000000000,
                        max_process_number=200, max_output_size=10000, exe_path="1.out",
                        input_path="1.in", output_path="1.out", error_path="1.out",
                        args=["1234"], env="1234", log_path="1.log",
                        seccomp_rule_so_path="1.so", uid=0, gid=0)

        with self.assertRaisesRegexp(ValueError, "env must be a list"):
            _judger.run(max_cpu_time=1000, max_real_time=2000, max_memory=1000000000,
                        max_process_number=200, max_output_size=10000, exe_path="1.out",
                        input_path="1.in", output_path="1.out", error_path="1.out",
                        args=["1234"], env={"k": "v"}, log_path="1.log",
                        seccomp_rule_so_path="1.so", uid=0, gid=0)

    def test_env_item_must_be_string(self):
        with self.assertRaisesRegexp(ValueError, "env item must be a string"):
            _judger.run(max_cpu_time=1000, max_real_time=2000, max_memory=1000000000,
                        max_process_number=200, max_output_size=10000, exe_path="1.out",
                        input_path="1.in", output_path="1.out", error_path="1.out",
                        args=["1234"], env=["1234", 1234], log_path="1.log",
                        seccomp_rule_so_path="1.so", uid=0, gid=0)

        with self.assertRaisesRegexp(ValueError, "env item must be a string"):
            _judger.run(max_cpu_time=1000, max_real_time=2000, max_memory=1000000000,
                        max_process_number=200, max_output_size=10000, exe_path="1.out",
                        input_path="1.in", output_path="1.out", error_path="1.out",
                        args=["1234"], env=["a=b", None], log_path="1.log",
                        seccomp_rule_so_path="1.so", uid=0, gid=0)

        with self.assertRaisesRegexp(ValueError, "env item must be a string"):
            _judger.run(max_cpu_time=1000, max_real_time=2000, max_memory=1000000000,
                        max_process_number=200, max_output_size=10000, exe_path="1.out",
                        input_path="1.in", output_path="1.out", error_path="1.out",
                        args=["1234"], env=[u"哈哈哈"], log_path="1.log",
                        seccomp_rule_so_path="1.so", uid=0, gid=0)

    def test_seccomp_rule_can_be_none(self):
        _judger.run(max_cpu_time=1000, max_real_time=2000, max_memory=1000000000,
                    max_process_number=200, max_output_size=10000, exe_path="/bin/ls",
                    input_path="/dev/null", output_path="/dev/null", error_path="/dev/null",
                    args=["12344"], env=["a=b"], log_path="/dev/null",
                    seccomp_rule_so_path="/usr/lib/judger/librule_c_cpp.so", uid=0, gid=0)

        _judger.run(max_cpu_time=1000, max_real_time=2000, max_memory=1000000000,
                    max_process_number=200, max_output_size=10000, exe_path="/bin/ls",
                    input_path="/dev/null", output_path="/dev/null", error_path="/dev/null",
                    args=["12344"], env=["a=b"], log_path="/dev/null",
                    seccomp_rule_so_path=None, uid=0, gid=0)

    def test_normal(self):
        config = self.config
        config["exe_path"] = self._compile("normal.c")
        config["input_path"] = self.make_input("judger_test")
        config["output_path"] = config["error_path"] = self.output_path()
        result = _judger.run(**config)
        output = "judger_test\nHello world"
        self.assertEqual(result["result"], _judger.SUCCESS)
        self.assertEqual(output, self.output_content(config["output_path"]))

    def test_args(self):
        config = self.config
        config["exe_path"] = self._compile("args.c")
        config["args"] = ["test", "hehe", "000"]
        config["output_path"] = config["error_path"] = self.output_path()
        result = _judger.run(**config)
        output = "argv[0]: /tmp/integration/args\nargv[1]: test\nargv[2]: hehe\nargv[3]: 000\n"
        self.assertEqual(result["result"], _judger.SUCCESS)
        self.assertEqual(output, self.output_content(config["output_path"]))

    def test_env(self):
        config = self.config
        config["exe_path"] = self._compile("env.c")
        config["output_path"] = config["error_path"] = self.output_path()
        result = _judger.run(**config)
        output = "judger_test\njudger\n"
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

    def test_re2(self):
        config = self.config
        config["exe_path"] = self._compile("re2.c")
        config["seccomp_rule_so_path"] = None
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.RUNTIME_ERROR)
        self.assertEqual(result["signal"], signal.SIGSEGV)

    def test_child_proc_cpu_time_limit(self):
        config = self.config
        config["exe_path"] = self._compile("child_proc_cpu_time_limit.c")
        config["seccomp_rule_so_path"] = None
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.CPU_TIME_LIMIT_EXCEEDED)

    def test_child_proc_real_time_limit(self):
        config = self.config
        config["exe_path"] = self._compile("child_proc_real_time_limit.c")
        config["seccomp_rule_so_path"] = None
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.REAL_TIME_LIMIT_EXCEEDED)
        self.assertEqual(result["signal"], signal.SIGKILL)

    def test_stdout_and_stderr(self):
        config = self.config
        config["exe_path"] = self._compile("stdout_stderr.c")
        config["output_path"] = config["error_path"] = self.output_path()
        result = _judger.run(**config)
        self.assertEqual(result["result"], _judger.SUCCESS)
        output = "stderr\n+++++++++++++++\n--------------\nstdout\n"
        self.assertEqual(output, self.output_content(config["output_path"]))

    def test_uid_and_gid(self):
        config = self.config
        config["exe_path"] = self._compile("uid_gid.c")
        config["output_path"] = config["error_path"] = self.output_path()
        config["seccomp_rule_so_path"] = None
        config["uid"] = 65534
        config["gid"] = 65534
        result = _judger.run(**config)
        print result
        self.assertEqual(result["result"], _judger.SUCCESS)
        output = "uid=65534(nobody) gid=65534(nogroup) groups=65534(nogroup)\nuid 65534\ngid 65534\n"
        self.assertEqual(output, self.output_content(config["output_path"]))
