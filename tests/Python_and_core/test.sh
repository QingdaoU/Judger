cd ../../bindings/Python
rm -r build
python setup.py install
rm -r build
python3 setup.py install
cd ../../tests/Python_and_core
python test.py
slepp 3
python3 test.py
