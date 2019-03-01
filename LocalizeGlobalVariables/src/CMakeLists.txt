cmake_minimum_required(VERSION 3.4.3)
project(main CXX)
enable_language(C)

if(MSVC)
    set(LLVM_DIR C:/Users/muraak/Documents/MyGarage/llvm-7.0.1.src/build/lib/cmake/llvm)

    find_package(LLVM REQUIRED CONFIG)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})

    set(CMAKE_CXX_COMPILER "clang++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
endif(MSVC)

if(APPLE)
    set(LLVM_DIR /Users/muraak/MyGarage/llvm-7.0.1.src/build/cmake/modules/)
    set(LLVM_INCLUDE_DIRS /Users/muraak/MyGarage/llvm-7.0.1.src/build/include)
    set(LLVM_LIBRARY_DIRS /Users/muraak/MyGarage/llvm-7.0.1.src/build/lib)

    find_package(LLVM REQUIRED CONFIG)
    add_definitions(${LLVM_DEFINITIONS})
    include_directories(${LLVM_INCLUDE_DIRS})
    link_directories(${LLVM_LIBRARY_DIRS})

    set(CMAKE_CXX_COMPILER "clang++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")


endif(APPLE)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")

add_executable(main main.cpp LocalizeGlobalVariableInSingleFunctionPass.cpp  LocalizeGlobalVariableInSingleFunctionPass.h)

if(MSVC)
    llvm_map_components_to_libnames(llvm_a_libs support core irreader scalaropts bitwriter)
    target_link_libraries(main ${llvm_a_libs})
endif(MSVC)

if(APPLE)
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
        LLVMBitWriter
	    LLVMIRReader
	    LLVMAsmParser)
endif(APPLE)

