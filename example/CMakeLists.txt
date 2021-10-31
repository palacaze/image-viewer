add_executable(viewer-example
    main.cpp
    rect-selection.cpp
    rect-selection.h
)

target_link_libraries(viewer-example Pal::ImageViewer)

set_target_properties(viewer-example PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    AUTOMOC ON
    AUTORCC ON
)

target_compile_options(viewer-example PRIVATE ${PAL_COMPILER_FLAGS})
