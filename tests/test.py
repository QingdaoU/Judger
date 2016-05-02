# coding=utf-8
import os
import pwd
import json
import judger
import shutil
from unittest import TestCase, main


class JudgerTest(TestCase):
    def setUp(self):
        self.tmp_path = "/tmp/judger_test"

    def compile_src(self, src_path, language, exe_path):
        if language == "c":
            return os.system("gcc %s -o %s" % (src_path, exe_path))
        elif language == "cpp":
            return os.system("g++ %s -o %s" % (src_path, exe_path))
        elif language == "java":
            pass
        else:
            raise ValueError("Invalid language")

    def test_run(self):
        shutil.rmtree(self.tmp_path, ignore_errors=True)
        os.mkdir(self.tmp_path)
        for i in os.listdir("."):
            try:
                int(i)
            except Exception:
                continue
            print "\n\nRunning test: ", i
            test_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), str(i))
            exe_path = os.path.join(self.tmp_path, str(i))
            config = json.loads(open(os.path.join(test_dir, "config")).read())
            self.assertEqual(self.compile_src(os.path.join(test_dir, "Main.c"), config.pop("language"), exe_path), 0)

            run_result = judger.run(path=exe_path,
                                    in_file=os.path.join(test_dir, "in"),
                                    out_file=os.path.join(self.tmp_path, str(i) + ".out"),
                                    **config)
            result = json.loads(open(os.path.join(test_dir, "result")).read())
            print run_result
            self.assertEqual(result["flag"], run_result["flag"])
            self.assertEqual(result["signal"], run_result["signal"])
            self.assertEqual(open(os.path.join(test_dir, "out")).read(),
                             open(os.path.join(self.tmp_path, str(i) + ".out")).read())
        self._judger_cpu_time_args_check()
        self._judger_memory_args_check()
        self._judger_exec_file_args_check()
        self._judger_in_file_args_check()
        self._judger_args_args_check()
        self._judger_env_args_check()
        self._judger_child_process_cpu_time_check()
        self._judger_user_args_check()

    def _judger_cpu_time_args_check(self):
        with self.assertRaisesRegexp(ValueError, "max_cpu_time must > 1 ms or unlimited"):
            judger.run(path="/bin/true", in_file="/dev/null",
                       out_file="/dev/null", max_cpu_time=0, max_memory=200000000,
                       env=["aaa=123"], use_sandbox=False, use_nobody=False)
        try: 
            judger.run(path="/bin/true", in_file="/dev/null",
                       out_file="/dev/null", max_cpu_time=judger.CPU_TIME_UNLIMITED, max_memory=200000000,
                       env=["aaa=123"], use_sandbox=False, use_nobody=False)
        except Exception as e:
            self.fail(e.message)

    def _judger_memory_args_check(self):
        with self.assertRaisesRegexp(ValueError, "max_memory must > 16M or unlimited"):
            judger.run(path="/bin/true", in_file="/dev/null",
                       out_file="/dev/null", max_cpu_time=1000, max_memory=100,
                       env=["aaa=123"], use_sandbox=True, use_nobody=True)
        try:
            judger.run(path="/bin/true", in_file="/dev/null",
                       out_file="/dev/null", max_cpu_time=1000, max_memory=judger.MEMORY_UNLIMITED,
                       env=["aaa=123"], use_sandbox=True, use_nobody=True)
        except Exception as e:
            self.fail(e.message)


    def _judger_exec_file_args_check(self):
        with self.assertRaisesRegexp(ValueError, "Exec file does not exist"):
            judger.run(path="/bin/not_found", in_file="/dev/null",
                       out_file="/dev/null", max_cpu_time=1000, max_memory=200000000,
                       env=["aaa=123"], use_sandbox=True, use_nobody=True)

    def _judger_in_file_args_check(self):
        with self.assertRaisesRegexp(ValueError, "in_file does not exist"):
            judger.run(path="/bin/true", in_file="/dev/xxx",
                       out_file="/dev/null", max_cpu_time=1000, max_memory=200000000,
                       env=["aaa=123"], use_sandbox=True, use_nobody=True)

    def _judger_args_args_check(self):
        with self.assertRaisesRegexp(ValueError, "args must be a list"):
            judger.run(path="/bin/ls", in_file="/dev/null",
                       out_file="/dev/null", max_cpu_time=1000, max_memory=200000000,
                       args=123, use_sandbox=True, use_nobody=True)

        with self.assertRaisesRegexp(ValueError, "arg in args must be a string"):
            judger.run(path="/bin/ls", in_file="/dev/null",
                       out_file="/dev/null", max_cpu_time=1000, max_memory=200000000,
                       args=["123", {"a": "b"}], use_sandbox=True, use_nobody=True)

        with self.assertRaisesRegexp(ValueError, "Number of args must < 95"):
            judger.run(path="/bin/ls", in_file="/dev/null",
                       out_file="/dev/null", max_cpu_time=1000, max_memory=200000000,
                       args=["123"] * 200, use_sandbox=True, use_nobody=True)

    def _judger_env_args_check(self):
        with self.assertRaisesRegexp(ValueError, "env must be a list"):
            judger.run(path="/bin/true", in_file="/dev/null",
                       out_file="/dev/null", max_cpu_time=1000, max_memory=200000000,
                       env=123, use_sandbox=True, use_nobody=True)

        with self.assertRaisesRegexp(ValueError, "env item must be a string"):
            judger.run(path="/bin/ls", in_file="/dev/null",
                       out_file="/dev/null", max_cpu_time=1000, max_memory=200000000,
                       env=["123", {"a": "b"}], use_sandbox=True, use_nobody=True)

        with self.assertRaisesRegexp(ValueError, "Number of env must < 95"):
            judger.run(path="/bin/true", in_file="/dev/null",
                       out_file="/dev/null", max_cpu_time=1000, max_memory=200000000,
                       env=["123=345"] * 200, use_sandbox=True, use_nobody=True)

    def _judger_user_args_check(self):
        os.setuid(pwd.getpwnam("nobody").pw_uid)
        with self.assertRaisesRegexp(ValueError, "Root user is required when use_nobody=True"):
            judger.run(path="/bin/ls", in_file="/dev/null",
                       out_file="/dev/null", max_cpu_time=2000, max_memory=200000000,
                       env=["aaa=123"], use_sandbox=True, use_nobody=True)

    # child process can not inherit timeout rules from parent process defined by setitimer, so we use setrlimit to
    # control child process max running time
    def _judger_child_process_cpu_time_check(self):
        try:
            max_cpu_time = 2000
            result = judger.run(path=os.path.join(self.tmp_path, "17"), in_file="/dev/null",
                                out_file="/dev/null", max_cpu_time=max_cpu_time, max_memory=200000000,
                                env=["aaa=123"], use_sandbox=False, use_nobody=True)
            expected_cpu_time = (max_cpu_time + 1000) / 1000 * 1000
            if result["cpu_time"] > expected_cpu_time * 1.1:
                self.fail("cpu time exceeded")
            if result["real_time"] >= max_cpu_time * 3:
                self.fail("real time exceeded")
        except Exception as e:
            self.fail(e.message)


if __name__ == "__main__":
    main()
