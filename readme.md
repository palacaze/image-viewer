# Pal Image Viewer #

A minimalistic Qt image viewer widget, with zoom and pixel color picking.

The widget can be installed as a shared library and used in CMake projects this way:

```cmake
find_package(PalImageViewer)

add_executable(Foo main.cpp)
target_link_libraries(Foo Pal::ImageViewer)
```
