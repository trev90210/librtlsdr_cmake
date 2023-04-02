cmake_minimum_required(VERSION 3.21)
project(rtlsdr)

# Only managed to compile for 64bit using mingw64 on msys2
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "Using x64 build of rtlsdr")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "x86 rtlsdr is unsupported")
else()
    message(FATAL_ERROR "sizeof(void*)=${CMAKE_SIZEOF_VOID_P} rtlsdr is unsupported")
endif()

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR})

add_library(rtlsdr::libusb SHARED IMPORTED)
set_target_properties(rtlsdr::libusb PROPERTIES 
    IMPORTED_IMPLIB   ${SRC_DIR}/lib/x64/libusb-1.0.lib
    IMPORTED_LOCATION ${SRC_DIR}/bin/x64/libusb-1.0.dll
)

add_library(rtlsdr::libwinpthread SHARED IMPORTED)
set_target_properties(rtlsdr::libwinpthread PROPERTIES 
    IMPORTED_IMPLIB   ${SRC_DIR}/lib/x64/libwinpthread-1.lib
    IMPORTED_LOCATION ${SRC_DIR}/bin/x64/libwinpthread-1.dll
)

add_library(rtlsdr::rtlsdr SHARED IMPORTED)  
set_target_properties(rtlsdr::rtlsdr PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES   ${SRC_DIR}/include
    IMPORTED_IMPLIB                 ${SRC_DIR}/lib/x64/librtlsdr.lib
    IMPORTED_LOCATION               ${SRC_DIR}/bin/x64/librtlsdr.dll
)
set_property(TARGET rtlsdr::rtlsdr PROPERTY INTERFACE_LINK_LIBRARIES rtlsdr::libusb rtlsdr::libwinpthread)