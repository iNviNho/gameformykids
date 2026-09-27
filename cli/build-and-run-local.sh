#/bin/bash

cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

cmake --build build --target gameformykids --config Debug --parallel 

./build/gameformykids.app/Contents/MacOS/gameformykids
