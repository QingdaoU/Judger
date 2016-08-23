# coding=utf-8
import os
import shutil
from unittest import TestCase


class RunResult(object):
    cpu_time_limited = 1
    real_time_limit_exceeded = 2
    memory_limit_exceeded = 3
    runtime_error = 4
    system_error = 5


class BaseTestCase(TestCase):
    def init_workspace(self, language):
        base_workspace = "/tmp"
        workspace = os.path.join(base_workspace, language)
        shutil.rmtree(workspace, ignore_errors=True)
        os.mkdir(workspace)
        return workspace

    def rand_str(self):
        return ''.join(map(lambda xx:(hex(ord(xx))[2:]), os.urandom(16)))