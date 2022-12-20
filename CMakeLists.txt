cmake_minimum_required( VERSION 3.12 FATAL_ERROR)
project(libjeff VERSION 0.1 )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

macro(print_build_mode)
  message(STATUS "The following build mode would be used: " ${BUILD_MODE})
endmacro()

option(CPU "no use at the moment" ON)
option(DATABASE "support for code interfacing with mongoDB (under development)")
option(EXTERNAL "support for unified IO with bzip2, gz and SFTP")
option(GPU "enable CUDA code in the library" ON)
option(MATHSTAT "enable the math and statistics code in the library" ON)
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
  message(FATAL_ERROR "Not yet implemented")
  list(APPEND BUILDMODES database)
endif()

if(ROBOTICS)
  find_package(serial)
  if(serial_FOUND)
    message(STATUS "serial found in ${serial_DIR}")
    set(serial_INCLUDE_DIR ${serial_DIR}/../../../include)
  
    set(SCIENTIFIC OFF)
    list(APPEND BUILDMODES robotics)
  endif()
endif()

if(MATHSTAT)
  list(APPEND BUILDMODES mathstat)
endif()
  
if(SCIENTIFIC)
  #check for gsl library
  if(NOT GSL_PREFIX)
    set(GSL_PREFIX /usr)
  endif()

  set(GSL_COMMAND ${GSL_PREFIX}/bin/gsl-config)
  execute_process(COMMAND ${GSL_COMMAND} --version OUTPUT_VARIABLE gsl_version RESULT_VARIABLE ret)
  if(${ret} EQUAL 0)
    execute_process(COMMAND ${GSL_COMMAND} --prefix OUTPUT_VARIABLE GSL_PREFIX RESULT_VARIABLE ret)
  else()
    set(GSL_COMMAND gsl-config)
    execute_process(COMMAND ${GSL_COMMAND} --version OUTPUT_VARIABLE gsl_version RESULT_VARIABLE ret)
    if(${ret} EQUAL 0)
      execute_process(COMMAND ${GSL_COMMAND} --prefix OUTPUT_VARIABLE GSL_PREFIX RESULT_VARIABLE ret)
    else()
       message(FATAL_ERROR "Unable to execute gsl-config. Unable to obtain the configuration for the GSL library.")
    endif()
  endif()

  if(${ret} EQUAL 0)
    message(STATUS "Finding gsl version = ${gsl_version}")
    
    if(NOT gsl_version VERSION_LESS 1.16)
      set(GSL_INC ${GSL_PREFIX}/include/)
      set(GSL_OK True)
    endif()
    
    if(gsl_version VERSION_LESS 1.16)
      message(FATAL_ERROR "GSL version incompatible.")
    endif()
  endif()

  
  #check for NETCDF
  if(NOT NETCDF_PREFIX)
    set(NETCDF_PREFIX /usr)
  endif()

  set(NETCDF_COMMAND ${NETCDF_PREFIX}/bin/nf-config)
  execute_process(COMMAND ${NETCDF_COMMAND} --version OUTPUT_VARIABLE netcdf_dump RESULT_VARIABLE ret)
  if(NOT ${ret} EQUAL 0)
    set(NETCDF_COMMAND nf-config)
    execute_process(COMMAND ${NETCDF_COMMAND} --version OUTPUT_VARIABLE netcdf_dump RESULT_VARIABLE ret)
  endif()
  
  if(${ret} EQUAL 0)
    string(FIND ${netcdf_dump} "netCDF-Fortran " netcdfloc)
    if(${netcdfloc} LESS 0)
      message(FATAL_ERROR "Response from nf-config not as expected")
    endif()
    math(EXPR netcdfloc ${netcdfloc}+15)
    string(SUBSTRING ${netcdf_dump} ${netcdfloc} -1 netcdf_version)
    message(STATUS "Finding netCDF version = ${netcdf_version}")

    if(NOT netcdf_version VERSION_LESS 4.2)
      execute_process(COMMAND ${NETCDF_COMMAND} --includedir OUTPUT_VARIABLE netcdf_INCLUDE_DIR)
      set(NETCDF_OK True)
    endif()
    
    if(netcdf_version VERSION_LESS 4.2)
      message(FATAL_ERROR "netCDF version incompatible.")
    endif()
  else()
    message(FATAL_ERROR "Unable to execute nf-config. Unable to obtain the configuration for the netCDF library.")
  endif()

  #check for NETCDF-C
  if(NOT NETCDFC_PREFIX)
    set(NETCDFC_PREFIX /usr)
  endif()

  set(NETCDFC_COMMAND ${NETCDFC_PREFIX}/bin/nc-config)
  execute_process(COMMAND ${NETCDFC_COMMAND} --version OUTPUT_VARIABLE netcdfc_dump RESULT_VARIABLE ret)
  if(NOT ${ret} EQUAL 0)
    set(NETCDFC_COMMAND nc-config)
    execute_process(COMMAND ${NETCDFC_COMMAND} --version OUTPUT_VARIABLE netcdfc_dump RESULT_VARIABLE ret)
  endif()

  if(${ret} EQUAL 0)
    string(FIND ${netcdfc_dump} "netCDF " netcdfcloc)
    if(${netcdfcloc} LESS 0)
      message(FATAL_ERROR "Response from nc-config not as expected")
    endif()
    math(EXPR netcdfcloc ${netcdfcloc}+7)
    string(SUBSTRING ${netcdfc_dump} ${netcdfcloc} -1 netcdfc_version)
    message(STATUS "Finding netCDF-C version = ${netcdfc_version}")
    
    if(NOT netcdfc_version VERSION_LESS 4.2.1.1)
      execute_process(COMMAND ${NETCDFC_COMMAND} --includedir OUTPUT_VARIABLE netcdfc_INCLUDE_DIR)
      set(NETCDFC_OK True)
    endif()

    if(netcdfc_version VERSION_LESS 4.2.1.1)
      message(FATAL_ERROR "netCDF-C version incompatible.")
    endif()
  else()
    message(FATAL_ERROR "Unable to execute nc-config. Unable to obtain the configuration for the netCDF-C library.")
  endif()

  #check for NETCDF-CXX
  if(NOT NETCDFCXX_PREFIX)
    set(NETCDFCXX_PREFIX /usr)
  endif()

  set(NETCDFCXX_COMMAND ${NETCDFCXX_PREFIX}/bin/ncxx4-config)
  execute_process(COMMAND ${NETCDFCXX_COMMAND} --version OUTPUT_VARIABLE netcdfc_dump RESULT_VARIABLE ret)
  if(NOT ${ret} EQUAL 0)
    set(NETCDFCXX_COMMAND ncxx4-config)
    execute_process(COMMAND ${NETCDFCXX_COMMAND} --version OUTPUT_VARIABLE netcdfc_dump RESULT_VARIABLE ret)
  endif()
  
  if(${ret} EQUAL 0)
    string(FIND ${netcdfcxx_dump} "netCDF-cxx4 " netcdfcxxloc)
    if(${netcdfcxxloc} LESS 0)
      message(FATAL_ERROR "Response from ncxx4-config not as expected")
    endif()
    math(EXPR netcdfcxxloc ${netcdfcxxloc}+12)
    string(SUBSTRING ${netcdfcxx_dump} ${netcdfcxxloc} -1 netcdfcxx_version)
    message(STATUS "Finding netCDF-CXX version = ${netcdfcxx_version}")
    
    if(NOT netcdfcxx_version VERSION_LESS 4.2)
      execute_process(COMMAND ${NETCDFCXX_COMMAND} --includedir OUTPUT_VARIABLE netcdfcxx_INCLUDE_DIR)
      set(NETCDFCXX_OK True)
    endif()
    
    if(netcdfcxx_version VERSION_LESS 4.2)
      message(FATAL_ERROR "netCDF-CXX version incompatible.")
    endif()
  else()
    message(FATAL_ERROR "Unable to execute ncxx4-config. Unable to obtain the configuration for the netCDF-CXX library.")
  endif()

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
  
  if(GSL_OK AND eccodes_FOUND AND civetweb_FOUND AND NETCDF_OK AND NETCDFC_OK AND NETCDFCXX_OK)
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
add_subdirectory(app app)
