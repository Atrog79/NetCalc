mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=${1:-$Release}
make -j$(nproc)
cd ..