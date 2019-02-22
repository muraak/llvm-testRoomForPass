cmake_minimum_required(VERSION 3.4.3)
project(prj_easypass CXX)
enable_language(C)

set(LLVM_DIR D:/myGarageD/llvm/llvm-7.0.0.src/build/lib/cmake/llvm)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(main main.cpp)
llvm_map_components_to_libnames(llvm_a_libs support core irreader scalaropts)
target_link_libraries(main ${llvm_a_libs})

