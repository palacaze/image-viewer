####### Library #######

add_library(ImageViewer SHARED
    ${PROJECT_BINARY_DIR}/include/pal/image-viewer-export.h
    ${PROJECT_SOURCE_DIR}/include/pal/image-viewer.h
    image-viewer.cpp
    image-viewer.qrc
)
add_library(Pal::ImageViewer ALIAS ImageViewer)

# Export header
include(GenerateExportHeader)
generate_export_header(ImageViewer
    BASE_NAME PAL_IMAGE_VIEWER
    EXPORT_FILE_NAME "${PROJECT_BINARY_DIR}/include/pal/image-viewer-export.h"
)

target_include_directories(ImageViewer
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(ImageViewer PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    AUTOMOC ON
    AUTORCC ON
    OUTPUT_NAME PalImageViewer
)

target_compile_options(ImageViewer PRIVATE ${PIV_COMPILER_FLAGS})

target_link_libraries(ImageViewer
    PUBLIC
        ${PIV_QT}::Core
        ${PIV_QT}::Gui
        ${PIV_QT}::Widgets
)


####### Library installation #######

install(
    TARGETS ImageViewer
    EXPORT PalImageViewerTargets
    RUNTIME
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
        COMPONENT PalImageViewerRuntime
    LIBRARY
        DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        COMPONENT PalImageViewerRuntime
        NAMELINK_COMPONENT PalImageViewerDevel
    ARCHIVE
        DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        COMPONENT PalImageViewerDevel
)

install(
    FILES ${PROJECT_BINARY_DIR}/include/pal/image-viewer-export.h
          ${PROJECT_SOURCE_DIR}/include/pal/image-viewer.h
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/pal"
    COMPONENT PalImageViewerDevel
)
