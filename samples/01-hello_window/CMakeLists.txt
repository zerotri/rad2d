cmake_minimum_required(VERSION 3.15)
project(01-hello_window)

include(ExternalProject)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(SDL2_VERSION "2.0.12")
    set(SDL2_LIBRARY_DIRS "${CMAKE_BINARY_DIR}/external/lib")
    set(SDL2_LIBRARIES "SDL2.lib")
    set(SDL2_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/external/include/")
    ExternalProject_Add(
        sdl2
        PREFIX ${CMAKE_BINARY_DIR}/external/
        DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/dl
        URL http://libsdl.org/release/SDL2-devel-${SDL2_VERSION}-VC.zip
        URL_MD5 C2262A2F2D4E8487B75B541BFEE3C2C4
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND 
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/external/src/sdl2/lib/x64   ${SDL2_LIBRARY_DIRS}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/external/src/sdl2/include   ${SDL2_INCLUDE_DIRS}/SDL2/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/external/src/sdl2/lib/x64/SDL2.dll   ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/SDL2.dll
        )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
# todo(Wynter): Add SDL package configuration for macOS
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
endif()

set(PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    )

add_executable("${PROJECT_NAME}" WIN32 ${PROJECT_SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/")
set_target_properties(${PROJECT_NAME} PROPERTIES 
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS ON
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED 14
    CXX_EXTENSIONS ON)

target_link_directories(${PROJECT_NAME} PUBLIC
    ${SDL2_LIBRARY_DIRS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${SDL2_LIBRARIES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
    "${SDL2_INCLUDE_DIRS}"
)