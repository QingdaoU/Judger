# coding=utf-8
import platform
from distutils.core import setup, Extension

setup(name='_judger',
      version='2.0',
      ext_modules=[Extension('_judger', sources=['_judger.c'], libraries=["dl"])])
