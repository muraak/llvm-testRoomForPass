set(LLVM_DIR /Users/muraak/MyGarage/llvm-7.0.1.src/build/cmake/modules)
set(LLVM_INCLUDE_DIRS /Users/muraak/MyGarage/llvm-7.0.1.src/build/include)
set(LLVM_LIBRARY_DIRS /Users/muraak/MyGarage/llvm-7.0.1.src/build/lib)

cmake_minimum_required(VERSION 3.1)

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")


project(main)

cmake_minimum_required(VERSION 3.4.3)

add_executable(main
    # List your source files here.
    main.cpp
)

target_link_libraries(main
    PUBLIC
    ncurses
    LLVMCore 
    LLVMSupport
    LLVMDemangle
    LLVMDebugInfoDWARF
    LLVMBinaryFormat
    lldReaderWriter
    LLVMBitReader
    LLVMIRReader
    LLVMAsmParser)

#llvm_map_components_to_libnames(llvm_a_libs support core irreader scalaropts)

message(STATUS "LLVM_LIBS" ${LLVM_LIBS})

target_link_libraries(main ${llvm_a_libs})

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(main PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(main PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
#if(APPLE)
#    set_target_properties(main PROPERTIES
#        LINK_FLAGS "-undefined dynamic_lookup"
#    )
#endif(APPLE)

