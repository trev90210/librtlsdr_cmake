cmake_minimum_required(VERSION 3.21)
project(rtlsdr)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(ARCH_DIR "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(ARCH_DIR "x86")
else()
    message(FATAL_ERROR "sizeof(void*)=${CMAKE_SIZEOF_VOID_P} rtlsdr is unsupported")
endif()

message(STATUS "Using ${ARCH_DIR} build of rtlsdr")

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR})
add_library(rtlsdr::rtlsdr SHARED IMPORTED)
set_target_properties(rtlsdr::rtlsdr PROPERTIES 
	INTERFACE_INCLUDE_DIRECTORIES   ${SRC_DIR}/include
	IMPORTED_IMPLIB                 ${SRC_DIR}/lib/${ARCH_DIR}/librtlsdr.dll.a
	IMPORTED_LOCATION               ${SRC_DIR}/bin/${ARCH_DIR}/librtlsdr.dll
)

# NOTE: rtlsdr_static doesn't include libusb or pthread symbols
add_library(rtlsdr::rtlsdr_static STATIC IMPORTED)
set_target_properties(rtlsdr::rtlsdr_static PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES   ${SRC_DIR}/include
	IMPORTED_LOCATION               ${SRC_DIR}/lib/${ARCH_DIR}/librtlsdr_static.a
	INTERFACE_COMPILE_DEFINITIONS   rtlsdr_STATIC
)