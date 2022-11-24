# Version 0.1 of libjeff (22 Nov 2022)

This is the C++/CUDA morphing code library developed by Jeffrey for all meteorological calculation in Hong Kong Observatory. The code was mostly closed for use in HKO before but now it is open to provide continual updates for HKO colleagues.

Initially plan to provide the code for scientific branch. Current Cmake support can be use to build the basic of the library.  For HKO users, most of the time you only need turn on the build mode for "scientific" and maybe "external" (for bziped read of GRIB files).

## To install,
1. clone the repository from Git.
2. install all of the dependency (listed below)
3. Execute "mkdir cmake"
4. Enter the directory cmake, execute "cmake ../"
5. Execute "cmake --build ."

## Dependency
The library assume the user is using g++ for compiling C++ codes and nvcc for CUDA codes
### For "Scientific" build mode
1. [GSL library version 1.16](https://www.gnu.org/software/gsl/), other version of GSL library may work but was not tested
2. [NetCDF Fortran version 4.2]
3. [NetCDF version 4.2.1.1]
4. [NetCDF-cxx4 version 4.2]
5. [ecCodes version 2.16.0](https://confluence.ecmwf.int/display/ECC), other version of ecCodes may work but was not well tested.  I only tested with version 2.16 and version 2.9
6. [civetweb v.1.12]
7. [BLAS]
8. [LAPACK version 3.5.0]
### For "XGBOOST" build mode
9. [XGBOOST] 
### For "external" build mode
10. BZIP2 - sudo apt-get install libz2-dev
11. ZLIB - usually pre-installed
12. libssh version 0.10.3 

~~## Behaviour
The current verion of the library would build into 3 different modes depending on the hardware/system configuration. If the library is built on a Raspberry Pi, it would assume it is for controlling robotic hardwares and would build in the "robotic" mode, which is not yet released at the moment.  This is because it is not possible to have enough resource to run most meteorological calculation on a raspberry Pi. For other computer, if no nvcc is detected, the library would be built in the "CPU only" mode and would not compile the CUDA codes in the library.  If nvcc is present, the code would compile into the GPU mode with all CUDA code available.~~

## Supported build modes
1. SCIENTIFIC (to be implemented)
2. DATABASE (to be implemented)
3. EXTERNAL (to be implemented)
4. XGBOOST (to be implemented)
5. GPU (automatic if nvcc >= 10.0 is detected. Can be suppressed by -DGPU=OFF)
6. ROBOTICS (private at the moment)

## Behaviour
The current version of the library would build depends on the build mode input during the configuration stage.  If no build mode was input, the library would only be built with the modules in the 'always' catergory. If ROBOTICS built mode is ON, the SCIENTIFIC built mode would be suppressed even if it is set to ON.

## Useful variables
CONCURRENT_BUILD_THREAD - the number of thread to be used for the make, i.e. the in X in "make -j X". Default value is 4.
GPU - set to OFF to suppress GPU build mode even when nvcc is detected
GSL_PREFIX - the prefix for the configuration of the libgsl. Default is /usr/


## TODO
* Adding the dependency check for the additional build modes.  The following built mode was done
1. GPU
2. 

## Known Issues


## To add additional modules
1. Create the code with header file named as .JEFFH and source file named as .JEFFC.  Put the code under the appropriate build mode directory

For use within Hong Kong Observatory (meteorological authority of Hong Kong SAR), MIT license applies. For others, the conditions in LICENSE.md applies.
