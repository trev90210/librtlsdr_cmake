get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
add_library(rtlsdr SHARED IMPORTED GLOBAL)  

# Only managed to compile for 64bit using mingw64 on msys2
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set_target_properties(rtlsdr PROPERTIES 
		INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include"
		IMPORTED_IMPLIB_RELEASE     "${_IMPORT_PREFIX}/lib/x64/librtlsdr.dll.a"
		IMPORTED_LOCATION_RELEASE   "${_IMPORT_PREFIX}/bin/x64/librtlsdr.dll")
    message(STATUS "Using x64 build of rtlsdr")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "x86 rtlsdr is unsupported")
endif()

set_property(TARGET rtlsdr APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)


