cmake_minimum_required( VERSION 3.12 FATAL_ERROR)
project(libjeff VERSION 0.1 )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CONCURRENT_BUILD_THREAD 4)

macro(print_build_mode)
  message(STATUS "The following build mode would be used: " ${BUILD_MODE})
endmacro()

option(CPU "no use at the moment" ON)
option(DATABASE "support for code interfacing with mongoDB (under development)")
option(EXTERNAL "support for unified IO with bzip2, gz and SFTP")
option(GPU "enable CUDA code in the library" ON)
option(ROBOTICS "compilation for robotics library, exclusive with scientific library (overrode SCIENTIFIC)")
option(SCIENTIFIC "compilation for scientific library, exclusive with robotic library (being overrode by ROBOTICS)")
option(XGBOOST "compilation of interfacing with XGBOOST")

if(CPU)
  list(APPEND BUILDMODES cpu)
endif()

if(GPU)
  include(CheckLanguage)
  check_language(CUDA)

  if(CMAKE_CUDA_COMPILER)
    message(STATUS "CMAKE_CUDA_COMPILER = ${CMAKE_CUDA_COMPILER}")
    message(STATUS "CMAKE_CUDA_COMPILER_ID = ${CMAKE_CUDA_COMPILER_ID}")
    message(STATUS "CMAKE_CUDA_COMPILER_VERSION = ${CMAKE_CUDA_COMPILER_VERSION}")

    if(NOT CMAKE_CUDA_COMPILER_VERSION VERSION_LESS 10)
      enable_language(CUDA)
      include(CheckLanguage)
      check_language(CUDA)
      
      list(APPEND BUILDMODES gpu)
    else()
      message(STATUS "CUDA COMPILER VERSION not supported")
    endif()
  else()
    message(STATUS "CUDA COMPILER NOT FOUND")
  endif()
endif()

if(DATABASE)
  list(APPEND BUILDMODES database)
endif()

if(SCIENTIFIC)
  find_package(LIBGSL 1.16)

  list(APPEND BUILDMODES scientific)
#  list(APPEND EXTRA_INCLUDES )
endif()

if(ROBOTICS)
  list(REMOTE_ITEM BUILDMODES scientific)
  list(APPEND BUILDMODES robotics)
endif()

if(EXTERNAL)
  list(APPEND BUILDMODES external)
endif()

if(XGBOOST)
  list(APPEND BUILDMODES xgboost)
endif()

list(JOIN BUILDMODES + BUILD_MODE)

add_subdirectory(build build)
file(MAKE_DIRECTORY src)
file(MAKE_DIRECTORY include)

print_build_mode()
