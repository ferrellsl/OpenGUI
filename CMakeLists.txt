# OpenGUI 5.5.14.
#
# Windows, with Visual Studio 2019 (32-bit: the library has inline assembler,
# which the 64-bit compiler doesn't take):
#   cmake -S . -B build -G "Visual Studio 16 2019" -A Win32
#   cmake --build build --config Release
#
# Windows, with MSYS2's gcc (64-bit), in an "MSYS2 MINGW64" shell:
#   cmake -S . -B build-msys -G Ninja
#   cmake --build build-msys
#
# Linux, with gcc (64-bit, drawing through X11):
#   cmake -S . -B build
#   cmake --build build -j
#
# (BUILD-VS2019.txt, BUILD-MSYS2.txt and BUILD-LINUX.txt say more.)
#
# Makes the libraries in lib/ - fgl (8-bit color), fgl16, fgl32 - with
# tinycfg, and the examples.
cmake_minimum_required(VERSION 3.15)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()
project(OpenGUI CXX)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# (single-configuration builds - make, ninja - put the libraries in lib/ too)
set(FG_LIB_DIRS ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
                ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib"
                ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib/debug")

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
  string(REPLACE "-DNDEBUG" "" CMAKE_CXX_FLAGS_${config} "${CMAKE_CXX_FLAGS_${config}}")
endforeach()

if(MSVC)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS _USE_MATH_DEFINES)   # (M_PI)
  add_compile_options(/W1 /EHsc /Zc:forScope-)   # (the old rule for "for (int i ...": i lives on after the loop)
  set(FG_QUIET /W0)
else()
  # The source is from 2005: the language as gcc 3 took it, and no promise
  # that it keeps to the rules today's optimizer relies on.
  set(CMAKE_CXX_STANDARD 98)
  set(CMAKE_CXX_EXTENSIONS ON)
  add_compile_options(-w -fno-strict-aliasing -fwrapv -fno-delete-null-pointer-checks)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
  set(FG_QUIET -w)
  if(MINGW)
    # (programs that need none of the compiler's own DLLs)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
  endif()
endif()

file(GLOB XML_SOURCES xml/*.cpp)
add_library(tinycfg STATIC ${XML_SOURCES})
target_include_directories(tinycfg PUBLIC xml)

# JPEG and PNG pictures (16- and 32-bit color only): from the sources of
# libjpeg 6b, libpng and zlib that GraphApp carries, if they are there.
#   -DGRAPHAPP_DIR=<folder>   where GraphApp is (default C:/GraphApp)
if(WIN32)
  set(GRAPHAPP_DEFAULT "C:/GraphApp")
else()
  set(GRAPHAPP_DEFAULT "$ENV{HOME}/dev/GraphApp")
endif()
set(GRAPHAPP_DIR "${GRAPHAPP_DEFAULT}" CACHE PATH "GraphApp's folder, for its libjpeg, libpng and libz sources")
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
  if(NOT MSVC)
    # (libpng 1.2's MMX routines are in a file for 32-bit gcc that GraphApp
    # doesn't carry: the plain C ones, then)
    target_compile_definitions(fgpng PUBLIC PNG_NO_MMX_CODE)
    target_compile_options(fgjpeg PRIVATE -w)
    target_compile_options(fgpng PRIVATE -w)
    target_compile_options(fgz PRIVATE -w)
  endif()
  set_target_properties(fgjpeg fgz fgpng PROPERTIES ${FG_LIB_DIRS})
  set(IMAGE_LIBS fgjpeg fgpng)
  set(IMAGE_DEFINES FG_JPEG FG_PNG)
  message(STATUS "JPEG and PNG pictures: from ${GRAPHAPP_DIR}")
else()
  message(STATUS "JPEG and PNG pictures: not built (no GraphApp sources at ${GRAPHAPP_DIR})")
endif()

# TrueType fonts: FreeType 2.13.3 (third_party/freetype, from freetype.org),
# built by its own CMake file with none of its optional parts.
set(TTF_LIBS "")
set(TTF_DEFINES "")
if(EXISTS "${CMAKE_SOURCE_DIR}/third_party/freetype/CMakeLists.txt")
  enable_language(C)
  foreach(part ZLIB BZIP2 PNG HARFBUZZ BROTLI)
    set(FT_DISABLE_${part} TRUE CACHE BOOL "" FORCE)
  endforeach()
  set(SKIP_INSTALL_ALL TRUE CACHE BOOL "" FORCE)
  add_subdirectory(third_party/freetype EXCLUDE_FROM_ALL)
  set(TTF_LIBS freetype)
  set(TTF_DEFINES FG_TTF)
  message(STATUS "TrueType fonts: FreeType from third_party/freetype")
endif()

# OpenGL: Mesa 7.0.4's software renderer and its off-screen driver, OSMesa
# (third_party/mesa, from mesa3d.org), as one static library.  The list of
# its sources is the one in Mesa's own Visual Studio project.
set(HAVE_MESA FALSE)
if(EXISTS "${CMAKE_SOURCE_DIR}/third_party/mesa/sources.txt")
  enable_language(C)
  file(STRINGS third_party/mesa/sources.txt MESA_LIST)
  set(MESA_SOURCES "")
  foreach(source ${MESA_LIST})
    list(APPEND MESA_SOURCES third_party/mesa/${source})
  endforeach()
  add_library(fgmesa STATIC ${MESA_SOURCES}
              third_party/mesa/src/mesa/drivers/common/driverfuncs.c
              third_party/mesa/src/mesa/drivers/osmesa/osmesa.c)
  target_include_directories(fgmesa PUBLIC third_party/mesa/include
    PRIVATE third_party/mesa/src/mesa third_party/mesa/src/mesa/glapi third_party/mesa/src/mesa/main
            third_party/mesa/src/mesa/shader third_party/mesa/src/mesa/shader/slang
            third_party/mesa/src/mesa/shader/grammar third_party/mesa/src/mesa/swrast)
  if(WIN32)
    target_compile_definitions(fgmesa PRIVATE WIN32 _LIB MESA_MINWARN _CRT_SECURE_NO_DEPRECATE)
    target_compile_definitions(fgmesa PUBLIC FG_MESA_STATIC)   # (see third_party/mesa/include/GL/gl.h)
  endif()
  if(NOT MSVC)
    # (C as it was in 2008, and none of the tricks that need a dynamic library)
    target_compile_options(fgmesa PRIVATE -std=gnu89 -fno-strict-aliasing -fwrapv -fcommon)
    if(NOT WIN32)
      target_link_libraries(fgmesa PUBLIC m)
    endif()
  endif()
  target_compile_options(fgmesa PRIVATE ${FG_QUIET})
  set_target_properties(fgmesa PROPERTIES ${FG_LIB_DIRS})
  set(HAVE_MESA TRUE)
  message(STATUS "OpenGL: Mesa from third_party/mesa")
endif()

if(WIN32)
  set(FG_OS win32)
  set(FG_OS_LIBS ddraw dxguid winmm)
  set(FG_OS_DEFINES "")
else()
  set(FG_OS linux)
  find_package(X11 REQUIRED)
  find_package(Threads REQUIRED)
  set(FG_OS_LIBS ${X11_Xext_LIB} ${X11_LIBRARIES} Threads::Threads ${CMAKE_DL_LIBS} m)
  set(FG_OS_DEFINES X11_DRIVER)
endif()
file(GLOB FGL_SOURCES src/*.cpp src/agg2/*.cpp src/${FG_OS}/*.cpp)
foreach(depth 8 16 32)
  if(depth EQUAL 8)
    set(name fgl)
  else()
    set(name fgl${depth})
  endif()
  add_library(${name} STATIC ${FGL_SOURCES})
  target_compile_definitions(${name} PRIVATE INTO_FGL PUBLIC BPP${depth} ${TTF_DEFINES} ${FG_OS_DEFINES})
  target_link_libraries(${name} PUBLIC ${TTF_LIBS})
  if(NOT depth EQUAL 8)     # (the library has no JPEG or PNG in palette color)
    target_compile_definitions(${name} PUBLIC ${IMAGE_DEFINES})
    target_link_libraries(${name} PUBLIC ${IMAGE_LIBS})
  endif()
  target_include_directories(${name} PUBLIC include/fastgl PRIVATE src src/agg2 src/${FG_OS})
  target_link_libraries(${name} PUBLIC tinycfg ${FG_OS_LIBS})
  set_target_properties(${name} PROPERTIES ${FG_LIB_DIRS})
endforeach()

# The examples, each built with 32-bit color beside its source (they look
# for their pictures in ../data).  fgxtest, which is for X11, is left out.
set(SKIPPED_EXAMPLES data fgxtest)
set(MESA_EXAMPLES gears geartrain morph3d)
if(NOT HAVE_MESA)
  list(APPEND SKIPPED_EXAMPLES ${MESA_EXAMPLES})
endif()
if(NOT TTF_LIBS)
  list(APPEND SKIPPED_EXAMPLES ttf)
endif()
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
      target_include_directories(ex_${example} PRIVATE include)
      target_link_libraries(ex_${example} PRIVATE fgl32)
      if(WIN32)
        target_include_directories(ex_${example} PRIVATE src/win32)   # (mydirent.h, which install.sh copies beside the headers)
        target_link_libraries(ex_${example} PRIVATE gdi32 user32)
      endif()
      if(example IN_LIST MESA_EXAMPLES)
        # (with OpenGUI's glue to OSMesa, built for the example's color depth)
        target_sources(ex_${example} PRIVATE mesa/fglmesa.cpp)
        target_include_directories(ex_${example} PRIVATE include/fastgl src)
        target_link_libraries(ex_${example} PRIVATE fgmesa)
        # (Mesa's gl.h declares the wgl functions on Windows, and wants
        # <windows.h> read first for their types)
        if(MSVC)
          target_compile_options(ex_${example} PRIVATE /FIwindows.h)
        elseif(WIN32)
          target_compile_options(ex_${example} PRIVATE -include windows.h)
        endif()
      endif()
      set_target_properties(ex_${example} PROPERTIES OUTPUT_NAME ${example}
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/examples/${example}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/examples/${example}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/examples/${example}"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/examples/${example}")
    endif()
  endif()
endforeach()
