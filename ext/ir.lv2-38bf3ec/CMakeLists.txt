# CMake configuration for building the IR plugin

cmake_minimum_required(VERSION 3.10)
project(IRPlugin)

# Set default install directories
set(PROJECT_DIR "C:/dev/proj/OrthodoxReverb/ext/ir.lv2-38bf3ec")
set(INSTDIR "${PROJECT_DIR}/installl/lv2")
set(BUNDLE "ir.lv2")

set(CMAKE_PREFIX_PATH "C:/msys64/mingw64")

set(GTK2_GTK_LIBRARY "C:/msys64/mingw64/lib/libgtk-win32-2.0.dll.a")
set(GTK2_GDK_LIBRARY "C:/msys64/mingw64/lib/libgdk-win32-2.0.dll.a")
set(GTK2_INCLUDE_DIR "C:/msys64/mingw64/include/gtk-2.0")
set(GTK2_GLIBCONFIG_INCLUDE_DIR "C:/msys64/mingw64/include/glib-2.0")
set(CUSTOM_INCLUDE_DIRS
    "C:/msys64/mingw64/include/gtk-2.0"
    "C:/msys64/mingw64/lib/gtk-2.0/include"
    "C:/msys64/mingw64/include/glib-2.0"
    "C:/msys64/mingw64/lib/glib-2.0/include"
    "C:/msys64/mingw64/include/cairo"
    "C:/msys64/mingw64/include/pango-1.0"
    "C:/msys64/mingw64/include/harfbuzz"
    "C:/dev/proj/OrthodoxReverb/ext/ir.lv2-38bf3ec/dep/libsndfile-1.2.2-win64/include"
    "C:/msys64/mingw64/include/gdk-pixbuf-2.0"
    "C:/msys64/mingw64/include/atk-1.0"
)

# Define sources
set(SOURCES
    ir.cc
    ir_utils.cc
    ir_gui.cc
    ir_meter.cc
    ir_modeind.cc
    ir_wavedisplay.cc
)

# Headers (for clarity, not strictly necessary)
set(HEADERS
    ir.h
    ir_utils.h
    ir_meter.h
    ir_modeind.h
    ir_wavedisplay.h
)

# Compiler flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2 -ffast-math")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2 -ffast-math")

# Dependencies

# BORIS: Let's hope it's backwards compatible
#find_package(GTK2 2.16 REQUIRED)
find_package(GTK2 2.24 REQUIRED)

find_package(Threads REQUIRED)

foreach(item IN LISTS CUSTOM_INCLUDE_DIRS)
    message(STATUS "-------------------BORIS: CUSTOM_INCLUDE_DIRS item: ${item}")
endforeach()

# Include directories
include_directories(
    .
    ${CUSTOM_INCLUDE_DIRS}
)

# Link libraries
set(LIBS
    ${GTK2_LIBRARIES}
    Threads::Threads
    zita-convolver
    samplerate
    ${CMAKE_SOURCE_DIR}/dep/libsndfile-1.2.2-win64/lib/sndfile.lib
)

# Shared library targets
add_library(ir MODULE ir.cc ir_utils.cc)
target_link_libraries(ir PRIVATE ${LIBS})

add_library(ir_gui MODULE ir_gui.cc ir_utils.cc ir_meter.cc ir_modeind.cc ir_wavedisplay.cc)
target_link_libraries(ir_gui PRIVATE ${LIBS})

# Installation rules
install(TARGETS ir ir_gui
    LIBRARY DESTINATION ${INSTDIR}/${BUNDLE}
)
install(FILES ir.ttl manifest.ttl DESTINATION ${INSTDIR}/${BUNDLE})
