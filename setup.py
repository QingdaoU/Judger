# coding=utf-8
from distutils.core import setup, Extension
setup(name='judger', 
      version='1.0', 
      ext_modules=[Extension('judger', sources=['judger.c', 'runner.c'], 
                                       libraries=['seccomp'])])
