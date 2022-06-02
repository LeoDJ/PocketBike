# Sadly doesn't work, as those newer cross-compilation toolchains apparently don't work anymore for ARMv6, despite the correct march/mcpu (will create startup code for ARMv7, causing the "Illegal instruction" exception)
# see https://github.com/Pro/raspi-toolchain/blob/master/README.md (3rd paragraph)
# This file will be kept for documentation's sake.

# Dependencies: sudo apt install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf


set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR armhf)

set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

# set(CMAKE_FIND_ROOT_PATH ../pi_rootfs)
# SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}")
# SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}")
# SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}")

# # Search for programs only in the build host directories
# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# # Search for libraries and headers only in the target directories
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

add_compile_options(-march=armv6+fp -marm -mfpu=vfp -mtp=soft)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")