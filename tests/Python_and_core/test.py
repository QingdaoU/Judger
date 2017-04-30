# coding=utf-8
from __future__ import print_function
import sys
import _judger
from unittest import TestCase, main

from testcase.integration.test import IntegrationTest
from testcase.seccomp.test import SeccompTest

ver = _judger.VERSION
print("Judger version %d.%d.%d" % ((ver >> 16) & 0xff, (ver >> 8) & 0xff, ver & 0xff))
print(sys.version)
main()
