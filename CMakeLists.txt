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

if(ROBOTICS)
  find_package(serial)
  if(serial_FOUND)
    message(STATUS "serial found in ${serial_DIR}")
    set(serial_INCLUDE_DIR ${serial_DIR}/../../../include)
  
    list(REMOTE_ITEM BUILDMODES scientific)
    list(APPEND BUILDMODES robotics)
  endif()
endif()

if(SCIENTIFIC)
  #check for gsl library
  if(NOT GSL_PREFIX)
    set(GSL_PREFIX /usr)
  endif()
  
  execute_process(COMMAND ${GSL_PREFIX}/bin/gsl-config --version OUTPUT_VARIABLE gsl_version)
  message(STATUS "Finding gsl version = ${gsl_version}")

  if(NOT gsl_version VERSION_LESS 1.16)
    set(GSL_INC ${GSL_PREFIX}/include/)
    set(GSL_OK True)
  endif()

  if(gsl_version VERSION_LESS 1.16)
    message(FATAL_ERROR "GSL version incompatible.")
  endif()

  #check for NETCDF

  #check for NETCDF-C

  #check for NETCDF-CXX

  #check for ECCODES
  find_package(eccodes)
  if(eccodes_FOUND)
    message(STATUS "ecCodes library found in ${eccodes_DIR}")
    set(eccodes_INCLUDE_DIR ${eccodes_DIR}/../../../include)
  endif()

  #check for CIVETWEB
  find_package(civetweb 1.11)
  if(civetweb_FOUND)
    message(STATUS "civetweb library found in ${civetweb_DIR}")
    set(civetweb_INCLUDE_DIR ${civetweb_DIR}/../../../include)
  endif()
  
  if(GSL_OK AND eccodes_FOUND AND civetweb_FOUND)
    #message(FATAL_ERROR "Not yet implemented")
    list(APPEND BUILDMODES scientific)
  endif()

#  list(APPEND EXTRA_INCLUDES )
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

  find_package(libssh 0.9.5)
  if(libssh_FOUND)
    message(STATUS "LIBSSH library found in ${libssh_DIR}")
    set(libssh_INCLUDE_DIR ${libssh_DIR}/../../../include)
  endif()
  
  if(ZLIB_FOUND AND BZIP2_FOUND AND libssh_FOUND)
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
