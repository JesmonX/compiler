
```bash
cmake -B build -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
cmake --build build

cd tests
python test.py ./../build/compiler lab1
python test.py ./../build/compiler lab2
ython test.py ../build/compiler lab3 --executor_path=./accipit-linux-x86_64-gnu
```
