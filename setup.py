# coding=utf-8
import platform
from distutils.core import setup, Extension


if platform.system() != 'Darwin':
    libraries = ['seccomp']
else:
    libraries = []
    
setup(name='judger', 
      version='1.0', 
      ext_modules=[Extension('judger', sources=['judger.c', 'runner.c'], 
                                       libraries=libraries)])
