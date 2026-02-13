cmake_minimum_required(VERSION 3.20)
project(MyMod VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

# Use vcpkg toolchain if available
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_win32.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_dx11.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES} )

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_BACKENDS_DIR}
)

add_library(my_mod MODULE
    src/my_mod.cpp
    src/d3d_renderer.cpp
    src/imgui_ui.cpp
 )

target_link_libraries(my_mod PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/winhttp.lib
    imgui
)

target_include_directories(my_mod PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(my_mod PROPERTIES
    PREFIX ""
    SUFFIX ".dll"
)
