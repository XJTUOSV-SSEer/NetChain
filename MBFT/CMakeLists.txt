cmake_minimum_required(VERSION 3.0.0)
project(basic VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

include_directories(./include/)

link_directories("/usr/lib/x86-64-linux-gnu" "/usr/local/lib")
add_executable(basic ./src/test.cpp ./src/Crypto_Primitives.cpp ./src/MBF.cpp ./src/MurmurHash3.cpp 
            ./src/Block.cpp ./src/MBFT.cpp ./src/Query.cpp)
target_link_libraries(basic libcrypto.so libpbc.so libgmp.so)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)