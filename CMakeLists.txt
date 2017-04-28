cmake_minimum_required(VERSION 3.6)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(pal-image-viewer VERSION 0.1 LANGUAGES CXX)

if(NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ON)
endif()


# compiler support
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

include(CheckCXXCompilerFlag)
macro(cmc_test_compiler_flag flag)
    string(REPLACE "-" "_" testname ${flag})
    string(TOUPPER ${testname} testname)
    check_cxx_compiler_flag("-${flag}" ${testname})
    if (${testname})
        list(APPEND PAL_COMPILER_FLAGS "-${flag}")
    endif()
endmacro()

cmc_test_compiler_flag("Wall")
cmc_test_compiler_flag("Wextra")
cmc_test_compiler_flag("fdiagnostics-color=always")
cmc_test_compiler_flag("pipe")

check_cxx_compiler_flag("-march=native" PAL_CHECK_MARCH_NATIVE)
if(PAL_CHECK_MARCH_NATIVE)
    add_compile_options($<$<CONFIG:Release>:-march=native>)
endif()

if(CMAKE_CXX_SIZEOF_DATA_PTR EQUAL 8)
    set(CMC_X64 1)
    set(CMC_X32 0)
else()
    set(CMC_X64 0)
    set(CMC_X32 1)
endif()


# Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 5.6 COMPONENTS Core Widgets Gui REQUIRED QUIET)

# Compile flag tweaks on every target
function(pal_set_common_properties target)
    target_compile_definitions(${target} PRIVATE $<$<CONFIG:Release>:NDEBUG>)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS NO)
    target_compile_options(${target} PRIVATE ${CMC_COMPILER_FLAGS})
endfunction()

# custom lib properties
function(pal_set_lib_properties target)
    pal_set_common_properties(${target})

    # handle symbol export
    string(REPLACE "-" "_" tname ${target})
    string(TOUPPER ${tname} tname)
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(${target} PRIVATE "${tname}_SHARED_LIBRARY")
    endif()
    target_compile_definitions(${target} PRIVATE "${tname}_BUILD_LIBRARY")
endfunction()

# custom app properties
function(pal_set_exe_properties target)
    pal_set_common_properties(${target})
endfunction()

add_subdirectory(src)
add_subdirectory(example)

