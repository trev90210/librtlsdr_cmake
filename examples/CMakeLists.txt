cmake_minimum_required(VERSION 3.0.0)
project(radio)

# Windows + MSYS = Install rtlsdr from submodule
# Ubuntu = Install rtlsdr from package manager
if (WIN32) 
    set(rtlsdr_DIR ${CMAKE_CURRENT_LIST_DIR}/../)
    find_package(rtlsdr CONFIG REQUIRED)
    set(RTLSDR_LIBS rtlsdr::rtlsdr)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(rtlsdr REQUIRED IMPORTED_TARGET librtlsdr)
    set(RTLSDR_LIBS PkgConfig::rtlsdr)
endif()

function(install_dlls target)
    if (WIN32)
        add_custom_command(
            TARGET ${target}
            POST_BUILD 
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${target}> 
            $<TARGET_FILE_DIR:${target}>
            COMMAND_EXPAND_LISTS
        )
    endif()
endfunction()

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/W3)
else()
    add_compile_options(-Wextra)
endif()

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR})
add_subdirectory(${SRC_DIR}/getopt)

add_executable(rtl_sdr ${SRC_DIR}/rtl_sdr.cpp)
target_link_libraries(rtl_sdr PRIVATE ${RTLSDR_LIBS} getopt) 
install_dlls(rtl_sdr)