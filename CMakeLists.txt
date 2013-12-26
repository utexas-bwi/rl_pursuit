cmake_minimum_required(VERSION 2.8.3)
project(rl_pursuit)

## Detect architecture
## http://www.cmake.org/pipermail/cmake/2011-June/045086.html
set(bitness 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(bitness 64)
endif()

## Find external pre-built libraries
foreach(lib gflags gtest json linear)
  find_library(${lib}_lib NAMES ${lib} PATHS libs/${bitness})
endforeach(lib)

catkin_package(
  INCLUDE_DIRS include src
  LIBRARIES rl_pursuit ${gflags_lib} ${gtest_lib} ${json_lib} ${linear_lib}
)

## Build
ADD_DEFINITIONS(-std=c++0x)
ADD_DEFINITIONS(-DCOMPILE_ARCH=${bitness})
ADD_DEFINITIONS(-fPIC)

include_directories(
  include
  src
)

## Temporarily commented as code is in flux 
file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
  src/rl_pursuit/common/*.cpp
  #src/rl_pursuit/controller/*.cpp 
  #src/rl_pursuit/factory/*.cpp
  #src/rl_pursuit/learning/*.cpp
  #src/rl_pursuit/model/*.cpp
  #src/rl_pursuit/planning/*.cpp
)

## The main rl_pursuit library
add_library(rl_pursuit ${SOURCES})
target_link_libraries(rl_pursuit
  ${gflags_lib} 
  ${gtest_lib} 
  ${json_lib} 
  ${linear_lib}
  blas
  rt
)

## Temporarily commented as code is in flux 

## Build all the test executables
## foreach(exec main runClassifier trainClassifier)
##   add_executable(${exec} src/main/${exec}.cpp)
##   target_link_libraries(${exec} rl_pursuit)
## endforeach(exec)
