
```bash
cmake -B build -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
cmake --build build

cd tests
python test.py ./../build/compiler lab1     
```
