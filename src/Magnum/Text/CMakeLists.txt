#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014
#             Vladimír Vondruš <mosra@centrum.cz>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

set(MagnumText_SRCS
    AbstractFont.cpp
    AbstractFontConverter.cpp
    DistanceFieldGlyphCache.cpp
    GlyphCache.cpp
    Renderer.cpp)
set(MagnumText_HEADERS
    AbstractFont.h
    AbstractFontConverter.h
    Alignment.h
    DistanceFieldGlyphCache.h
    GlyphCache.h
    Renderer.h
    Text.h

    visibility.h)

if(MAGNUM_BUILD_DEPRECATED)
    set(MagnumText_HEADERS ${MagnumText_HEADERS}
        TextRenderer.h)
endif()

add_library(MagnumText ${SHARED_OR_STATIC} ${MagnumText_SRCS})
set_target_properties(MagnumText PROPERTIES DEBUG_POSTFIX "-d")
if(BUILD_STATIC_PIC)
    # TODO: CMake 2.8.9 has this as POSITION_INDEPENDENT_CODE property
    set_target_properties(MagnumText PROPERTIES COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
endif()
target_link_libraries(MagnumText Magnum MagnumTextureTools)

install(TARGETS MagnumText
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumText_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Text)

if(WITH_FONTCONVERTER)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
                   ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

    include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})

    add_executable(magnum-fontconverter fontconverter.cpp)

    if(UNIX AND NOT TARGET_GLES)
        target_link_libraries(magnum-fontconverter MagnumText Magnum MagnumWindowlessGlxApplication ${X11_LIBRARIES})
    elseif(WIN32)
        target_link_libraries(magnum-fontconverter MagnumText Magnum MagnumWindowlessWglApplication)
    else()
        message(FATAL_ERROR "magnum-fontconverter is not available on this platform. Set WITH_FONTCONVERTER to OFF to suppress this warning.")
    endif()

    install(TARGETS magnum-fontconverter DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})
endif()

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()
