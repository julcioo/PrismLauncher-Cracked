if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64 OR CMAKE_SYSTEM_PROCESSOR STREQUAL amd64)
        set(OBJCOPY_TARGET "elf64-x86-64")
        set(OBJCOPY_ARCH "x86_64")
    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL i686)
        set(OBJCOPY_TARGET "elf32-i386")
        set(OBJCOPY_ARCH "i386")
    else()
        message(AUTHOR_WARNING "This is not a platform that would support testing nbt++")
        return()
    endif()
else()
    message(AUTHOR_WARNING "This is not a platform that would support testing nbt++")
    return()
endif()

enable_testing()
find_package(CxxTest REQUIRED)

include_directories(${libnbt++_SOURCE_DIR}/include)
include_directories(${CXXTEST_INCLUDE_DIR})

function(build_data out_var)
    set(result)
    foreach(in_f ${ARGN})
        set(out_f "${CMAKE_CURRENT_BINARY_DIR}/testfiles/${in_f}.obj")
        add_custom_command(
            COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/testfiles"
            COMMAND ${CMAKE_OBJCOPY}  --prefix-symbol=_  --input-target=binary  --output-target=${OBJCOPY_TARGET} "${in_f}" "${out_f}"
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/testfiles/${in_f}
            OUTPUT ${out_f}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testfiles/
            VERBATIM
        )
        SET_SOURCE_FILES_PROPERTIES(
            ${out_f}
            PROPERTIES
            EXTERNAL_OBJECT true
            GENERATED true
        )
        list(APPEND result ${out_f})
    endforeach()
    set(${out_var} "${result}" PARENT_SCOPE)
endfunction()

build_data(DATA_OBJECTS
    bigtest.nbt
    bigtest.zlib
    bigtest_corrupt.nbt
    bigtest_eof.nbt
    bigtest_uncompr
    errortest_eof1
    errortest_eof2
    errortest_neg_length
    errortest_noend
    littletest_uncompr
    toplevel_string
    trailing_data.zlib
)
add_library(NbtTestData STATIC ${DATA_OBJECTS})

#Specifies that the directory containing the testfiles get copied when the target is built
function(use_testfiles target)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/testfiles ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

function(stop_warnings target)
    target_compile_options(${target} PRIVATE
        -Wno-unused-value
        -Wno-self-assign-overloaded
    )
endfunction()

if(NBT_USE_ZLIB)
    set(EXTRA_TEST_LIBS ${ZLIB_LIBRARY})
endif()

CXXTEST_ADD_TEST(nbttest nbttest.cpp ${CMAKE_CURRENT_SOURCE_DIR}/nbttest.h)
target_link_libraries(nbttest ${NBT_NAME})
stop_warnings(nbttest)

CXXTEST_ADD_TEST(endian_str_test endian_str_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/endian_str_test.h)
target_link_libraries(endian_str_test ${NBT_NAME})
stop_warnings(endian_str_test)

CXXTEST_ADD_TEST(read_test read_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/read_test.h)
target_link_libraries(read_test ${NBT_NAME} ${EXTRA_TEST_LIBS} NbtTestData)
stop_warnings(read_test)

CXXTEST_ADD_TEST(write_test write_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/write_test.h)
target_link_libraries(write_test ${NBT_NAME} ${EXTRA_TEST_LIBS} NbtTestData)
stop_warnings(write_test)

if(NBT_USE_ZLIB)
    CXXTEST_ADD_TEST(zlibstream_test zlibstream_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/zlibstream_test.h)
    target_link_libraries(zlibstream_test ${NBT_NAME} ${EXTRA_TEST_LIBS} NbtTestData)
    stop_warnings(zlibstream_test)
endif()

add_executable(format_test format_test.cpp)
target_link_libraries(format_test ${NBT_NAME})
add_test(format_test format_test)
stop_warnings(format_test)
