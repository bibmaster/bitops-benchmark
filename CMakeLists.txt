cmake_minimum_required(VERSION 3.1.2)
project(bitops-benchmark)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Threads REQUIRED)

function(addTest test)
    add_executable(${test} ./src/${test}.cpp)
    target_link_libraries(${test} CONAN_PKG::google-benchmark Threads::Threads)
    if(WIN32)
        target_link_libraries(${test} shlwapi.lib)
    endif()
endfunction()

addTest(bm-log10)
addTest(bm-int-tostring)