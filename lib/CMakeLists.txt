###############################################################################
#  Copyright (c) 2016-2023 Joel de Guzman
#  Copyright (c) 2020 Michał Urbański
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
cmake_minimum_required(VERSION 3.7.2...3.15.0)

###############################################################################
# Artist library

set(ARTIST_BUILD_EXAMPLES OFF CACHE BOOL "Artist build examples")
set(ARTIST_BUILD_TESTS OFF CACHE BOOL "Artist build tests")

if (ELEMENTS_QUARTZ_2D)
   set(ARTIST_QUARTZ_2D ON CACHE BOOL "Artist graphics backend option")
elseif (ELEMENTS_SKIA)
   set(ARTIST_SKIA ON CACHE BOOL "Artist graphics backend option")
endif()

add_subdirectory(artist)

###############################################################################
# Sources (and Resources)

set(ELEMENTS_SOURCES
   src/element/button.cpp
   src/element/child_window.cpp
   src/element/composite.cpp
   src/element/dial.cpp
   src/element/drag_and_drop.cpp
   src/element/element.cpp
   src/element/floating.cpp
   src/element/flow.cpp
   src/element/gallery/button.cpp
   src/element/gallery/check_box.cpp
   src/element/gallery/icon_button.cpp
   src/element/gallery/menu.cpp
   src/element/gallery/radio_button.cpp
   src/element/gallery/slide_switch.cpp
   src/element/gallery/tab.cpp
   src/element/gallery/thumbwheel.cpp
   src/element/grid.cpp
   src/element/image.cpp
   src/element/label.cpp
   src/element/layer.cpp
   src/element/list.cpp
   src/element/menu.cpp
   src/element/misc.cpp
   src/element/popup.cpp
   src/element/port.cpp
   src/element/progress_bar.cpp
   src/element/proxy.cpp
   src/element/range_slider.cpp
   src/element/slider.cpp
   src/element/text.cpp
   src/element/thumbwheel.cpp
   src/element/tile.cpp
   src/element/tooltip.cpp
   src/support/draw_utils.cpp
   src/support/text_utils.cpp
   src/support/receiver.cpp
   src/support/theme.cpp
   src/support/payload.cpp
   src/view.cpp
)

set(ELEMENTS_HEADERS
   include/elements.hpp
   include/elements/app.hpp
   include/elements/base_view.hpp
   include/elements/element.hpp
   include/elements/element/align.hpp
   include/elements/element/button.hpp
   include/elements/element/composite.hpp
   include/elements/element/dial.hpp
   include/elements/element/drag_and_drop.hpp
   include/elements/element/element.hpp
   include/elements/element/floating.hpp
   include/elements/element/flow.hpp
   include/elements/element/gallery.hpp
   include/elements/element/gallery/button.hpp
   include/elements/element/gallery/caption.hpp
   include/elements/element/gallery/check_box.hpp
   include/elements/element/gallery/thumbwheel.hpp
   include/elements/element/gallery/dialog.hpp
   include/elements/element/gallery/group.hpp
   include/elements/element/gallery/icon_button.hpp
   include/elements/element/gallery/menu.hpp
   include/elements/element/gallery/message_box.hpp
   include/elements/element/gallery/notebook.hpp
   include/elements/element/gallery/pane.hpp
   include/elements/element/gallery/radio_button.hpp
   include/elements/element/gallery/tab.hpp
   include/elements/element/gallery/text_entry.hpp
   include/elements/element/grid.hpp
   include/elements/element/image.hpp
   include/elements/element/indirect.hpp
   include/elements/element/label.hpp
   include/elements/element/layer.hpp
   include/elements/element/list.hpp
   include/elements/element/margin.hpp
   include/elements/element/menu.hpp
   include/elements/element/misc.hpp
   include/elements/element/popup.hpp
   include/elements/element/port.hpp
   include/elements/element/progress_bar.hpp
   include/elements/element/proxy.hpp
   include/elements/element/range_slider.hpp
   include/elements/element/selectable.hpp
   include/elements/element/size.hpp
   include/elements/element/slider.hpp
   include/elements/element/text.hpp
   include/elements/element/thumbwheel.hpp
   include/elements/element/tile.hpp
   include/elements/element/tracker.hpp
   include/elements/support.hpp
   include/elements/support/context.hpp
   include/elements/support/draw_utils.hpp
   include/elements/support/icon_ids.hpp
   include/elements/support/receiver.hpp
   include/elements/support/text_utils.hpp
   include/elements/support/theme.hpp
   include/elements/view.hpp
   include/elements/window.hpp
)

if (APPLE)
   set(ELEMENTS_HOST
      host/macos/app.mm
      host/macos/base_view.mm
      host/macos/key.mm
      host/macos/window.mm
   )
elseif (ELEMENTS_HOST_UI_LIBRARY STREQUAL "gtk")
   set(ELEMENTS_HOST
      host/gtk3/app.cpp
      host/gtk3/base_view.cpp
      host/gtk3/key.cpp
      host/gtk3/window.cpp
   )
elseif (WIN32)
   set(ELEMENTS_HOST
      host/windows/utils.hpp
      host/windows/app.cpp
      host/windows/base_view.cpp
      host/windows/key.cpp
      host/windows/window.cpp
      host/windows/drag_and_drop.cpp
   )
endif()

source_group("Source Files\\elements"
   FILES
   ${ELEMENTS_SOURCES}
)

source_group("Source Files\\elements_host"
   FILES
   ${ELEMENTS_HOST}
)

source_group("Header Files\\elements"
   FILES
   ${ELEMENTS_HEADERS}
)

add_subdirectory(infra)
add_library(elements)

target_sources(elements
   PRIVATE ${ELEMENTS_SOURCES} ${ELEMENTS_HOST}
   PUBLIC ${ELEMENTS_HEADERS}
)

target_include_directories(elements PUBLIC include)
target_link_libraries(elements PUBLIC cycfi::infra)
if(CMAKE_CXX_STANDARD EQUAL 20)
   target_compile_features(elements PUBLIC cxx_std_20)
else()
   target_compile_features(elements PUBLIC cxx_std_17)
endif()

if(ELEMENTS_ENABLE_LTO)
   set_target_properties(elements PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if (NOT MSVC)
   find_package(PkgConfig REQUIRED)
endif()

###############################################################################
# Get rid of certain warnings, and make msvc support utf-8 without adding bom

target_compile_options(elements PRIVATE
   $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -ftemplate-backtrace-limit=0>
   $<$<CXX_COMPILER_ID:Clang>:-Wall -Wpedantic -ftemplate-backtrace-limit=0>
   $<$<CXX_COMPILER_ID:AppleClang>:-Wall -Wpedantic -ftemplate-backtrace-limit=0>
   $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4244 /wd4305 /wd4996 /wd4267 /wd4018 /wd4456 /utf-8>
)

if (MSVC)
   # Kinda silly to have to do this: https://bit.ly/2ZXjzzn
   STRING(REGEX REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

if (APPLE)
   # Not sure why APPLE is not picking the target_compile_options above
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -ftemplate-backtrace-limit=0")
endif()

###############################################################################
# Fontconfig

if (MSVC)
   target_include_directories(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/fontconfig/include)

   if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	  target_link_libraries(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/fontconfig/x64/fontconfig.lib)
   else()
	  target_link_libraries(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/fontconfig/x86/fontconfig.lib)
   endif()
elseif (APPLE OR UNIX OR WIN32)
   pkg_check_modules(fontconfig REQUIRED IMPORTED_TARGET fontconfig)
   target_link_libraries(elements PUBLIC PkgConfig::fontconfig)
endif()

###############################################################################
# Freetype

if (MSVC)
   target_include_directories(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/freetype/include)

   if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	  target_link_libraries(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/freetype/win64/freetype.lib)
   else()
	  target_link_libraries(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/freetype/win32/freetype.lib)
   endif()
elseif (APPLE OR UNIX OR WIN32)
   pkg_check_modules(freetype2 REQUIRED IMPORTED_TARGET freetype2)
   target_link_libraries(elements PUBLIC PkgConfig::freetype2)
endif()

###############################################################################
# GTK3

if (ELEMENTS_HOST_UI_LIBRARY STREQUAL "gtk")
   pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)
   target_link_libraries(elements PRIVATE PkgConfig::GTK3)
endif()

###############################################################################
# Asio headers
target_include_directories(elements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/asio/asio/include)

###############################################################################
# Global options

if (APPLE)
   target_compile_definitions(elements PRIVATE
      ELEMENTS_CLASS_PREFIX=${ELEMENTS_CLASS_PREFIX}
   )
   target_compile_options(elements PUBLIC "-fobjc-arc")
elseif (WIN32)
   target_compile_definitions(elements PUBLIC
      WIN32_LEAN_AND_MEAN
      NOMINMAX
      _UNICODE
      BOOST_DATE_TIME_NO_LIB
      BOOST_REGEX_NO_LIB
   )
   set_property(TARGET elements PROPERTY
      MSVC_RUNTIME_LIBRARY "MultiThreaded"
   )
endif()

if(ELEMENTS_HOST_UI_LIBRARY STREQUAL "gtk")
    target_compile_definitions(elements PUBLIC ELEMENTS_HOST_UI_LIBRARY_GTK)
elseif(ELEMENTS_HOST_UI_LIBRARY STREQUAL "cocoa")
    if(NOT APPLE)
        message(FATAL_ERROR "Only macOS supports ELEMENTS_HOST_UI_LIBRARY=cocoa")
    endif()
    target_compile_definitions(elements PUBLIC ELEMENTS_HOST_UI_LIBRARY_COCOA)
elseif(ELEMENTS_HOST_UI_LIBRARY STREQUAL "win32")
    if(NOT WIN32)
        message(FATAL_ERROR "Only Windows supports ELEMENTS_HOST_UI_LIBRARY=win32")
    endif()
    target_compile_definitions(elements PUBLIC ELEMENTS_HOST_UI_LIBRARY_WIN32)
    if(ELEMENTS_HOST_ONLY_WIN7)
        target_compile_definitions(elements PRIVATE ELEMENTS_HOST_ONLY_WIN7)
        message(STATUS "Windows 7 compatibility enabled")
    elseif(MINGW)
        # Required by MinGW to enable SetProcessDpiAwareness (Windows 8.1+)
        # and GetDpiForWindow (Windows 10+) API
        target_compile_definitions(elements PRIVATE _WIN32_WINNT=0x0A00)
    endif()
else()
    message(FATAL_ERROR "Invalid ELEMENTS_HOST_UI_LIBRARY=${ELEMENTS_HOST_UI_LIBRARY}. Set gtk, cocoa or win32.")
endif()

###############################################################################
# Libraries and linking

if (UNIX OR (WIN32 AND NOT MSVC))
   find_package(Threads)
   target_link_libraries(elements PUBLIC Threads::Threads artist)
endif()

# ASIO requires this
if (WIN32 AND NOT MSVC)
   target_link_libraries(elements PUBLIC ws2_32 artist)
endif()

# Replace missing Shcore library removed by WIN32_LEAN_AND_MEAN
if (WIN32)
   target_link_libraries(elements PUBLIC Shcore artist)
endif()

add_library(cycfi::elements ALIAS elements)
