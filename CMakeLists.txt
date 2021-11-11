cmake_minimum_required(VERSION 3.12)

project(PalImageViewer VERSION 0.2 LANGUAGES CXX)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(PIV_STANDALONE ON)
    set(BUILD_SHARED_LIBS ON)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

####### Options #######

# Allow forcing Qt version
set(PIV_PREFERRED_QT_VERSION "" CACHE STRING "Preferred Qt major version number to use.")


####### Compilation Flags #######

include(CheckCXXCompilerFlag)

macro(pal_test_compiler_flag flag)
    string(REPLACE "-" "_" testname ${flag})
    string(TOUPPER ${testname} testname)
    check_cxx_compiler_flag("${flag}" ${testname})
    if (${testname})
        list(APPEND PIV_COMPILER_FLAGS "${flag}")
    endif()
endmacro()

pal_test_compiler_flag("-Wall")
pal_test_compiler_flag("-Wextra")
pal_test_compiler_flag("-fdiagnostics-color=always")

####### Find packages #######

# Attempt Qt6
if ((NOT PIV_PREFERRED_QT_VERSION) OR (PIV_PREFERRED_QT_VERSION STREQUAL "6"))
    find_package(Qt6 COMPONENTS Core Widgets Gui QUIET)
    set(PIV_QT Qt${Qt6_VERSION_MAJOR})
endif()

# Or Qt5
if ((NOT Qt6_FOUND) AND (NOT PIV_PREFERRED_QT_VERSION STREQUAL "6"))
    find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED QUIET)
    set(PIV_QT Qt${Qt5_VERSION_MAJOR})
endif()

if (PIV_QT STREQUAL Qt)
    message(FATAL_ERROR "Qt not found")
endif()

####### Targets #######

add_subdirectory(src)

if (PIV_STANDALONE)
    add_subdirectory(example)
endif()


####### Package Installation #######

if (PIV_STANDALONE)
    install(
        EXPORT PalImageViewerTargets
        FILE PalImageViewerTargets.cmake
        NAMESPACE Pal::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/PalImageViewer"
        COMPONENT PalImageViewerDevel
     )

    # Setup Qt dependency check
    set(PIV_QT_DEPENDENCY "${PIV_QT} COMPONENTS Core Gui Widgets")
    configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/PalImageViewerConfig.cmake.in"
        "${PROJECT_BINARY_DIR}/PalImageViewerConfig.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/PalImageViewer"
    )

    write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/PalImageViewerConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY ExactVersion
    )

    install(
        FILES "${PROJECT_BINARY_DIR}/PalImageViewerConfig.cmake"
              "${PROJECT_BINARY_DIR}/PalImageViewerConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/PalImageViewer"
        COMPONENT PalImageViewerDevel
    )

    export(
        TARGETS ImageViewer
        NAMESPACE Pal::
        FILE PalImageViewerTargets.cmake
    )
endif()
