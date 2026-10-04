#/bin/bash

cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build build --target gameformykids --config Debug --parallel 

./build/gameformykids.app/Contents/MacOS/gameformykids
