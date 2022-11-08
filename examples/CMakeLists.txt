cmake_minimum_required(VERSION 3.0.0)
project(radio)

set(rtlsdr_DIR ${CMAKE_SOURCE_DIR}/../)
find_package(rtlsdr CONFIG REQUIRED)

add_executable(rtl_sdr rtl_sdr.c convenience.c getopt.c)
target_link_libraries(rtl_sdr PRIVATE rtlsdr) 
target_include_directories(rtl_sdr PUBLIC ${rtlsdr_DIR})

# vcpkg.cmake has internal stuff that autogenerates this
# we have to do this manually
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_custom_command(TARGET rtl_sdr 
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${rtlsdr_DIR}/bin/x64/
        $<TARGET_FILE_DIR:rtl_sdr>)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "x86 rtlsdr is unsupported")
endif()