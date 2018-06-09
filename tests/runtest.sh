#! /bin/bash
set -ex
dir=$PWD
python -V
gcc -v
g++ -v

for py in python2 python3; do
	cd $dir
	rm -rf build && mkdir build && cd build && cmake ..
	make
	make install
	cd ../bindings/Python && rm -rf build
	$py setup.py install
	cd ../../tests/Python_and_core && $py test.py
done
