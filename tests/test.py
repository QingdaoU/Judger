# coding=utf-8
import os
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
            exe_path = os.path.join("/tmp/judger_test", str(i))
            config = json.loads(open(os.path.join(test_dir, "config")).read())
            self.assertEqual(self.compile_src(os.path.join(test_dir, "Main.c"), config.pop("language"), exe_path), 0)

            run_result = judger.run(path=exe_path,
                                    in_file=os.path.join(test_dir, "in"),
                                    out_file=os.path.join(self.tmp_path, str(i) + ".out"),
                                    **config)
            result = json.loads(open(os.path.join(test_dir, "result")).read())
            self.assertEqual(result["flag"], run_result["flag"])
            self.assertEqual(result["signal"], run_result["signal"])
            self.assertEqual(open(os.path.join(test_dir, "out")).read(), open(os.path.join(self.tmp_path, str(i) + ".out")).read())


if __name__ == "__main__":
    main()
