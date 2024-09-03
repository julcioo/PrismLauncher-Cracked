cmake_minimum_required(VERSION 3.9.4)
project(libnbt++
    VERSION 2.3)

# supported configure options
option(NBT_BUILD_SHARED "Build shared libraries" OFF)
option(NBT_USE_ZLIB "Build additional zlib stream functionality" ON)
option(NBT_BUILD_TESTS "Build the unit tests. Requires CxxTest." ON)

if(NBT_NAME)
    message("Using override nbt++ name: ${NBT_NAME}")
else()
    set(NBT_NAME nbt++)
endif()

# hide this from includers.
set(BUILD_SHARED_LIBS ${NBT_BUILD_SHARED})

include(GenerateExportHeader)

set(NBT_SOURCES
    src/endian_str.cpp
    src/tag.cpp
    src/tag_compound.cpp
    src/tag_list.cpp
    src/tag_string.cpp
    src/value.cpp
    src/value_initializer.cpp

    src/io/stream_reader.cpp
    src/io/stream_writer.cpp

    src/text/json_formatter.cpp)

set(NBT_SOURCES_Z
    src/io/izlibstream.cpp
    src/io/ozlibstream.cpp)

if(NBT_USE_ZLIB)
    find_package(ZLIB REQUIRED)
    list(APPEND NBT_SOURCES ${NBT_SOURCES_Z})
    add_definitions("-DNBT_HAVE_ZLIB")
endif()

add_library(${NBT_NAME} ${NBT_SOURCES})
target_include_directories(${NBT_NAME} PUBLIC include ${CMAKE_CURRENT_BINARY_DIR})

# Install it
if(DEFINED NBT_DEST_DIR)
    install(
        TARGETS ${NBT_NAME}
        ARCHIVE DESTINATION ${LIBRARY_DEST_DIR}
        RUNTIME DESTINATION ${LIBRARY_DEST_DIR}
        LIBRARY DESTINATION ${LIBRARY_DEST_DIR}
    )
endif()

if(NBT_USE_ZLIB)
    target_link_libraries(${NBT_NAME} ZLIB::ZLIB)
endif()
set_property(TARGET ${NBT_NAME} PROPERTY CXX_STANDARD 11)
generate_export_header(${NBT_NAME} BASE_NAME nbt)

if(${BUILD_SHARED_LIBS})
    set_target_properties(${NBT_NAME} PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN 1)
endif()

if(NBT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
