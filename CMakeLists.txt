cmake_minimum_required(VERSION 2.8)

PROJECT(CubeFun)
SET(VTK_DIR /Users/adman634/Downloads/VTK-build/)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(CubeFun MACOSX_BUNDLE CubeFun)



SET(CMAKE_CXX_LINK_FLAGS "-framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework Carbon -framework OpenGL libportaudio.a")
if(VTK_LIBRARIES)
  target_link_libraries(CubeFun ${VTK_LIBRARIES})
else()
  target_link_libraries(CubeFun vtkHybrid)
endif()

