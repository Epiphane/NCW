#
# Copyright (c) 2014-2016 Enrico M. Crisostomo
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.
#
include_directories(.. src/libfswatch)
add_definitions(-DHAVE_CONFIG_H)

set(LIB_SOURCE_FILES
        src/libfswatch/c/cevent.cpp
        src/libfswatch/c/cevent.h
        src/libfswatch/c/cfilter.h
        src/libfswatch/c/cmonitor.h
        src/libfswatch/c/error.h
        src/libfswatch/c/libfswatch.cpp
        src/libfswatch/c/libfswatch.h
        src/libfswatch/c/libfswatch_log.cpp
        src/libfswatch/c/libfswatch_log.h
        src/libfswatch/c/libfswatch_types.h
        src/libfswatch/cpp/event.cpp
        src/libfswatch/cpp/event.hpp
        src/libfswatch/cpp/filter.hpp
        src/libfswatch/cpp/filter.cpp
        src/libfswatch/cpp/libfswatch_exception.cpp
        src/libfswatch/cpp/libfswatch_exception.hpp
        src/libfswatch/cpp/libfswatch_map.hpp
        src/libfswatch/cpp/libfswatch_set.hpp
        src/libfswatch/cpp/monitor.cpp
        src/libfswatch/cpp/monitor.hpp
        src/libfswatch/cpp/monitor_factory.cpp
        src/libfswatch/cpp/monitor_factory.hpp
        src/libfswatch/cpp/path_utils.cpp
        src/libfswatch/cpp/path_utils.hpp
        src/libfswatch/cpp/poll_monitor.cpp
        src/libfswatch/cpp/poll_monitor.hpp
        src/libfswatch/cpp/string/string_utils.cpp
        src/libfswatch/cpp/string/string_utils.hpp
        src/libfswatch/gettext.h
        src/libfswatch/gettext_defs.h
        src/libfswatch/libfswatch_config.h)

INCLUDE(CheckIncludeFiles)

CHECK_INCLUDE_FILES(sys/inotify.h HAVE_SYS_INOTIFY)

if (HAVE_SYS_INOTIFY_H)
    set(LIB_SOURCE_FILES
            ${LIB_SOURCE_FILES}
            src/libfswatch/cpp/inotify_monitor.cpp
            src/libfswatch/cpp/inotify_monitor.hpp)
endif (HAVE_SYS_INOTIFY_H)

CHECK_INCLUDE_FILES(sys/event.h HAVE_SYS_EVENT_H)

if (HAVE_SYS_EVENT_H)
    set(LIB_SOURCE_FILES
            ${LIB_SOURCE_FILES}
            src/libfswatch/cpp/kqueue_monitor.cpp
            src/libfswatch/cpp/kqueue_monitor.hpp)
endif (HAVE_SYS_EVENT_H)

CHECK_INCLUDE_FILES(port.h HAVE_PORT_H)

if (HAVE_PORT_H)
    set(LIB_SOURCE_FILES
            ${LIB_SOURCE_FILES}
            src/libfswatch/cpp/fen_monitor.cpp
            src/libfswatch/cpp/fen_monitor.hpp)
endif (HAVE_PORT_H)

if (WIN32)
    CHECK_INCLUDE_FILES(sys/cygwin.h HAVE_CYGWIN)

    if (HAVE_CYGWIN)
        set(LIB_SOURCE_FILES
                ${LIB_SOURCE_FILES}
                src/libfswatch/cpp/windows/win_directory_change_event.cpp
                src/libfswatch/cpp/windows/win_directory_change_event.hpp
                src/libfswatch/cpp/windows/win_error_message.cpp
                src/libfswatch/cpp/windows/win_error_message.hpp
                src/libfswatch/cpp/windows/win_handle.cpp
                src/libfswatch/cpp/windows/win_handle.hpp
                src/libfswatch/cpp/windows/win_paths.cpp
                src/libfswatch/cpp/windows/win_paths.hpp
                src/libfswatch/cpp/windows/win_strings.cpp
                src/libfswatch/cpp/windows/win_strings.hpp
                src/libfswatch/cpp/windows_monitor.cpp
                src/libfswatch/cpp/windows_monitor.hpp)
        add_definitions(-DHAVE_WINDOWS)
    endif (HAVE_CYGWIN)
endif (WIN32)

if (APPLE)
    CHECK_INCLUDE_FILES(CoreServices/CoreServices.h HAVE_FSEVENTS_FILE_EVENTS)

    if (HAVE_FSEVENTS_FILE_EVENTS)
        find_library(CORESERVICES_LIBRARY CoreServices)
        set(EXTRA_LIBS ${CORESERVICES_LIBRARY})

        set(LIB_SOURCE_FILES
                ${LIB_SOURCE_FILES}
                src/libfswatch/cpp/fsevents_monitor.cpp
                src/libfswatch/cpp/fsevents_monitor.hpp)

    endif (HAVE_FSEVENTS_FILE_EVENTS)
endif (APPLE)

add_library(libfswatch ${LIB_SOURCE_FILES})
target_include_directories(libfswatch PUBLIC src)
target_link_libraries(libfswatch ${CORESERVICES_LIBRARY})
