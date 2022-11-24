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
option(SCIENTIFIC "compilation for scientific library, exclusive with robotic library (being overrode by ROBOTICS)" ON)
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
  message(FATAL_ERROR "Not yet implemented")
  list(APPEND BUILDMODES database)
endif()

if(SCIENTIFIC)
  #check for gsl library
  if(NOT GSL_PREFIX)
    set(GSL_PREFIX /usr)
  endif()
  
  execute_process(COMMAND ${GSL_PREFIX}/bin/gsl-config --version OUTPUT_VARIABLE gsl_version)
  message(STATUS "Finding gsl version = ${gsl_version}")

  #check for NETCDF

  #check for NETCDF-C

  #check for NETCDF-CXX

  #check for ECCODES

  #check for CIVETWEB

  if(NOT gsl_version VERSION_LESS 1.16)
    set(GSL_INC ${GSL_PREFIX}/include/)
    set(GSL_OK True)
  endif()

  if(GSL_OK AND BLAS_FOUND)
    message(FATAL_ERROR "Not yet implemented")
    list(APPEND BUILDMODES scientific)
  endif()

  if(gsl_version VERSION_LESS 1.16)
    message(FATAL_ERROR "GSL version incompatible.")
  endif()
#  list(APPEND EXTRA_INCLUDES )
endif()

if(ROBOTICS)
  list(REMOTE_ITEM BUILDMODES scientific)
  message(FATAL_ERROR "Not yet implemented")
  list(APPEND BUILDMODES robotics)
endif()

if(EXTERNAL)
  find_package(ZLIB)
  if(ZLIB_FOUND)
    message(STATUS "ZLIB found in ${ZLIB_INCLUDE_DIRS}")
  endif()
    
  find_package(BZip2)
  if(BZIP2_FOUND)
    message(STATUS "BZIP2 library found in ${BZIP2_INCLUDE_DIRS}")
  endif()
  
  if(ZLIB_FOUND AND BZIP2_FOUND)
    message(FATAL_ERROR "Not yet implemented")
    list(APPEND BUILDMODES external)
  endif()
endif()

if(XGBOOST)
  message(FATAL_ERROR "Not yet implemented")
  list(APPEND BUILDMODES xgboost)
endif()

list(JOIN BUILDMODES + BUILD_MODE)

add_subdirectory(build build)
file(MAKE_DIRECTORY src)
file(MAKE_DIRECTORY include)

print_build_mode()
