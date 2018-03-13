cmake_minimum_required(VERSION 3.1.0)
# set target name
set(TARGET_NAME win32tools)
#===================================================================================================
# Set default build type explicitly to Release
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build Type." FORCE )
# Project name
project(${TARGET_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
# This setting is useful for providing JSON file used by CodeLite for code completion
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
# -----------------------------------------------------------------------------
# Default output directories
#------------------------------------------------------------------------------
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
#=======================================================================
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOURCE_FILES ${SOURCE_DIR}/main.cpp
${SOURCE_DIR}/Network/IocpTcpServer.cpp
${SOURCE_DIR}/Network/NamedPipe.cpp
${SOURCE_DIR}/Network/NetworkSocket.cpp
${SOURCE_DIR}/Network/Packet.cpp
${SOURCE_DIR}/Network/Socket.cpp
${SOURCE_DIR}/Tools/DriveInfo.cpp
${SOURCE_DIR}/Tools/HaxHelper.cpp
${SOURCE_DIR}/Tools/Inject.cpp
${SOURCE_DIR}/Tools/ProcessID.cpp
${SOURCE_DIR}/Tools/Tools.cpp)

add_library(${TARGET_NAME} STATIC ${SOURCE_FILES})
# GCC will automatically add the prefix lib
if(CMAKE_COMPILER_IS_GNUCXX)
	set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${TARGET_NAME} ) 
endif()
# Compiler options
target_compile_options(${TARGET_NAME} PUBLIC -O2 -Wall -pedantic -Wextra)
#=======================================================================
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
#=======================================================================
target_link_libraries(${TARGET_NAME} ${LINK_OPTIONS})