# coding=utf-8
import platform
from distutils.core import setup, Extension

setup(name='_judger',
      version='2.1',
      ext_modules=[Extension('_judger', 
                             sources=['_judger.c'], 
                             extra_link_args=["-L", "/usr/lib/judger", "-l", "judger", "-Wl,-rpath=/usr/lib/judger"])])
