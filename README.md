# Version 0.2 of libjeff (29 Nov 2022)

This is the C++/CUDA morphing code library developed by Jeffrey for all meteorological calculation in Hong Kong Observatory. The code was mostly closed for use in HKO before but now it is open to provide continual updates for HKO colleagues.

This initial release only provides the code for scientific branch. For HKO users, most of the time you only need turn on the build mode for "scientific" and maybe "external" (for bziped read of GRIB files) with "-DSCIENTIFIC=ON" and "-DEXTERNAL=ON" when executing the cmake command.  See "Supported build modes" below for info.  No HKO software executables are included in this repository. 

## To install,
1. clone the repository from Git.
2. install all of the dependency (listed below)
3. Execute "mkdir cmake"
4. Enter the directory cmake, execute "cmake ../".  Note that additional variable would be needed for the cmake to find the dependent packages. 
5. Execute "cmake --build ."
6. The header files are in the "include" directory while the libjeff.a is in the "src" directory. 

## Supported build modes
Build modes can be turned on/off by the argument -D{build modes}=ON/OFF.
1. SCIENTIFIC (For the equations for meteorological computation and also the support for various grid types in handling NWP/satellite/RADAR/LIDAR data.)
2. DATABASE (Not yet released)
3. EXTERNAL
4. MATHSTAT (For numerical routines. ON by default. Can be suppressed by -DMATHSTAT=OFF)
5. XGBOOST (Not yet released)
6. GPU (automatic if nvcc >= 10.0 is detected. Can be suppressed by -DGPU=OFF)
7. ROBOTICS (private at the moment)

## Behaviour
The current version of the library would build depends on the build mode input during the configuration stage.  If no build mode was input, the library would only be built with the modules in the 'always' catergory. If ROBOTICS built mode is ON, the SCIENTIFIC built mode would be suppressed even if it is set to ON.

## Dependency
The library assume the user is using g++ for compiling C++ codes and nvcc for CUDA codes
### For "SCIENTIFIC" build mode
1. [GSL library version 1.16](https://www.gnu.org/software/gsl/), other version of GSL library may work but was not tested
2. [NetCDF Fortran version 4.2]
3. [NetCDF version 4.2.1.1] (implicitly needs HDF5)
4. [NetCDF-cxx4 version 4.2]
5. [ecCodes version 2.16.0](https://confluence.ecmwf.int/display/ECC), other version of ecCodes may work but was not well tested.  I only tested with version 2.16 and version 2.9
6. [civetweb v.1.12]
### For "XGBOOST" build mode
7. [XGBOOST] 
### For "EXTERNAL" build mode
8. BZIP2 - sudo apt-get install libz2-dev
9. ZLIB - usually pre-installed
10. libssh version 0.9.5
### For "ROBOTICS" build mode
11. [serial version 1.2.1](https://github.com/wjwwood/serial)

## Useful variables
* CONCURRENT_BUILD_THREAD - the number of thread to be used for the make, i.e. the in X in "make -j X". Default value is 4.
* GPU - set to OFF to suppress GPU build mode even when nvcc is detected
* GSL_PREFIX - the prefix for the configuration of the libgsl. Default is /usr/
* NETCDF_PREFIX - the prefix for the configuration of the netCDF library. Default is /usr/
* NETCDFC_PREFIX - the prefix for the configuration of the netCDF-C library. Default is /usr/
* NETCDFCXX_PREFIX - the prefix for the configuration of the netCDF-CXX library. Default is /usr/

## TODO

## Known Issues
NETCDF read fucntion yet to be compatible with the sftp mode

## To add additional modules
1. Create the code with header file named as .JEFFH and source file named as .JEFFC.  Put the code under the appropriate build mode directory

For use within Hong Kong Observatory (meteorological authority of Hong Kong SAR), MIT license applies. For others, the conditions in LICENSE.md applies.

## Future Plan
This repository will go quiet for a bit of time as the initial release is considered done unless there is comments from HKO colleagues. Future release will be on the availability of the ROBOTICS build mode. Test cases will be added from time to time. The main branch of this repository will be moved back to my own storage system (which is still on transit) once I bring my system back online (expect no later than Aug 2023). 
