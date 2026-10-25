cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
./cmake_build.sh
gdb build/sandbox/sandbox
