# coding=utf-8
import _judger
from unittest import TestCase


class IntegrationTest(TestCase):
    def setUp(self):
        print "Running", self._testMethodName

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
                    max_process_number=200, max_output_size=10000, exe_path="1.out",
                    input_path="1.in", output_path="1.out", error_path="1.out",
                    args=["12344"], env=["a=b"], log_path="1.log",
                    seccomp_rule_so_path="1.so", uid=0, gid=0)

        _judger.run(max_cpu_time=1000, max_real_time=2000, max_memory=1000000000,
                    max_process_number=200, max_output_size=10000, exe_path="1.out",
                    input_path="1.in", output_path="1.out", error_path="1.out",
                    args=["12344"], env=["a=b"], log_path="1.log",
                    seccomp_rule_so_path=None, uid=0, gid=0)
