# OpenGUI 5.5.14 with Visual Studio 2019 (32-bit: the library has inline
# assembler, which the 64-bit compiler doesn't take).
#
#   cmake -S . -B build -G "Visual Studio 16 2019" -A Win32
#   cmake --build build --config Release
#
# Makes lib/fgl.lib (8-bit color), fgl16.lib, fgl32.lib and tinycfg.lib, and
# the examples.
cmake_minimum_required(VERSION 3.15)
project(OpenGUI CXX)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# include/fastgl/fgversion.h, as compile.sh makes it from the file "version"
file(STRINGS version VERSION_LINES)
list(GET VERSION_LINES 0 FG_MAJOR)
list(GET VERSION_LINES 1 FG_MINOR)
list(GET VERSION_LINES 2 FG_PATCH)
list(GET VERSION_LINES 3 FG_BUILD)
file(WRITE include/fastgl/fgversion.h
  "#define FG_VERSION \"${FG_MAJOR}.${FG_MINOR}.${FG_PATCH}.${FG_BUILD}\"\n#define FG_MAJOR ${FG_MAJOR}\n#define FG_MINOR ${FG_MINOR}\n#define FG_PATCH ${FG_PATCH}\n")

# The library does real work inside assert() - assert(name = new char[n]) -
# so NDEBUG, which removes assert's argument altogether, must never be
# defined: CMake defines it for every configuration but Debug.
foreach(config RELEASE RELWITHDEBINFO MINSIZEREL)
  string(REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_${config} "${CMAKE_CXX_FLAGS_${config}}")
endforeach()

add_compile_definitions(_CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS _USE_MATH_DEFINES)   # (M_PI)
add_compile_options(/W1 /EHsc /Zc:forScope-)   # (the old rule for "for (int i ...": i lives on after the loop)

file(GLOB XML_SOURCES xml/*.cpp)
add_library(tinycfg STATIC ${XML_SOURCES})
target_include_directories(tinycfg PUBLIC xml)

# JPEG and PNG pictures (16- and 32-bit color only): from the sources of
# libjpeg 6b, libpng and zlib that GraphApp carries, if they are there.
#   -DGRAPHAPP_DIR=<folder>   where GraphApp is (default C:/GraphApp)
set(GRAPHAPP_DIR "C:/GraphApp" CACHE PATH "GraphApp's folder, for its libjpeg, libpng and libz sources")
set(IMAGE_LIBS "")
set(IMAGE_DEFINES "")
if(EXISTS "${GRAPHAPP_DIR}/src/libjpeg/jpeglib.h" AND EXISTS "${GRAPHAPP_DIR}/src/libpng/png.h" AND EXISTS "${GRAPHAPP_DIR}/src/libz/zlib.h")
  enable_language(C)
  # GraphApp's copies of these libraries include its apptypes.h, which has
  # nothing to stop it being read twice (C++ objects to that).  They get
  # this one instead, which reads the real one once.
  file(WRITE "${CMAKE_BINARY_DIR}/graphapp/apptypes.h"
    "#ifndef OPENGUI_GRAPHAPP_APPTYPES_H
#define OPENGUI_GRAPHAPP_APPTYPES_H
#include \"${GRAPHAPP_DIR}/src/apptypes.h\"
#endif
")
  file(GLOB JPEG_SOURCES "${GRAPHAPP_DIR}/src/libjpeg/*.c")
  file(GLOB PNG_SOURCES "${GRAPHAPP_DIR}/src/libpng/*.c")
  file(GLOB Z_SOURCES "${GRAPHAPP_DIR}/src/libz/*.c")
  add_library(fgjpeg STATIC ${JPEG_SOURCES} other/graphapp_alloc.c)
  target_include_directories(fgjpeg PUBLIC "${GRAPHAPP_DIR}/src/libjpeg" "${CMAKE_BINARY_DIR}/graphapp")   # (GraphApp's copies include its apptypes.h: see below)
  add_library(fgz STATIC ${Z_SOURCES})
  target_include_directories(fgz PUBLIC "${GRAPHAPP_DIR}/src/libz" "${CMAKE_BINARY_DIR}/graphapp")   # (GraphApp's copies include its apptypes.h: see below)
  add_library(fgpng STATIC ${PNG_SOURCES})
  target_include_directories(fgpng PUBLIC "${GRAPHAPP_DIR}/src/libpng")
  target_link_libraries(fgpng PUBLIC fgz)
  set_target_properties(fgjpeg fgz fgpng PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib"
                                                    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib/debug")
  set(IMAGE_LIBS fgjpeg fgpng)
  set(IMAGE_DEFINES FG_JPEG FG_PNG)
  message(STATUS "JPEG and PNG pictures: from ${GRAPHAPP_DIR}")
else()
  message(STATUS "JPEG and PNG pictures: not built (no GraphApp sources at ${GRAPHAPP_DIR})")
endif()

file(GLOB FGL_SOURCES src/*.cpp src/agg2/*.cpp src/win32/*.cpp)
foreach(depth 8 16 32)
  if(depth EQUAL 8)
    set(name fgl)
  else()
    set(name fgl${depth})
  endif()
  add_library(${name} STATIC ${FGL_SOURCES})
  target_compile_definitions(${name} PRIVATE INTO_FGL PUBLIC BPP${depth})
  if(NOT depth EQUAL 8)     # (the library has no JPEG or PNG in palette color)
    target_compile_definitions(${name} PUBLIC ${IMAGE_DEFINES})
    target_link_libraries(${name} PUBLIC ${IMAGE_LIBS})
  endif()
  target_include_directories(${name} PUBLIC include/fastgl PRIVATE src src/agg2 src/win32)
  target_link_libraries(${name} PUBLIC tinycfg ddraw dxguid winmm)
  set_target_properties(${name} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib"
                                           ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib/debug")
endforeach()

# The examples, each built with 32-bit color beside its source (they look
# for their pictures in ../data).  Those that need what isn't built here -
# OpenGL through Mesa, FreeType fonts - are left out.
set(SKIPPED_EXAMPLES data gears geartrain morph3d ttf fgxtest)
file(GLOB EXAMPLE_DIRS RELATIVE "${CMAKE_SOURCE_DIR}/examples" examples/*)
foreach(example ${EXAMPLE_DIRS})
  if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/examples/${example}" AND NOT example IN_LIST SKIPPED_EXAMPLES)
    # (the source named after the example, if there is one: font.cpp includes its 12x24.cpp itself)
    if(EXISTS "${CMAKE_SOURCE_DIR}/examples/${example}/${example}.cpp")
      set(EXAMPLE_SOURCES examples/${example}/${example}.cpp)
    else()
      file(GLOB EXAMPLE_SOURCES examples/${example}/*.cpp)
    endif()
    if(EXAMPLE_SOURCES)
      add_executable(ex_${example} ${EXAMPLE_SOURCES})
      target_include_directories(ex_${example} PRIVATE include src/win32)   # (mydirent.h, which install.sh copies beside the headers)
      target_link_libraries(ex_${example} PRIVATE fgl32 gdi32 user32)
      set_target_properties(ex_${example} PROPERTIES OUTPUT_NAME ${example}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/examples/${example}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/examples/${example}"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/examples/${example}")
    endif()
  endif()
endforeach()
