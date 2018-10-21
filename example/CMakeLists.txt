add_executable(viewer-example
    main.cpp
    rect-selection.cpp
    rect-selection.h
)

pal_set_exe_properties(viewer-example)

target_link_libraries(viewer-example
    Pal::ImageViewer
    Qt5::Gui
    Qt5::Widgets
)

