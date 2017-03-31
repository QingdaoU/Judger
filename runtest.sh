apt-get update
apt-get install cmake liblua5.3-dev
rm -rf build && mkdir build && cd build && cmake .. && make && make install
cd ../bindings/Python && rm -rf build && python setup.py install || exit 1
cd ../../tests/Python_and_core &&  python test.py
