# coding=utf-8
import os
import judger


base_path = os.path.dirname(os.path.abspath(__file__))


print judger.run(path=os.path.join(base_path, "test"),
                 in_file=os.path.join(base_path, "in"),
                 out_file=os.path.join(base_path, "out"),
                 max_cpu_time=2000,
                 max_memory=200000000,
                 args=["1", "2", "####"], 
                 env=[],
                 use_sandbox=True)

