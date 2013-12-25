cmake_minimum_required(VERSION 2.8.3)
project(rl_pursuit)

catkin_package(
  INCLUDE_DIRS include src
  LIBRARIES rl_pursuit
)

## Detect architecture
set(bitness 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(bitness 64)
endif()

## Build
ADD_DEFINITIONS(-std=c++0x)
ADD_DEFINITIONS(-DCOMPILE_ARCH=${bitness})
ADD_DEFINITIONS(-fPIC)

include_directories(
  include
  src
)
link_directories(
  libs/${bitness}
)

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
  src/rl_pursuit/common/*.cpp
  #src/rl_pursuit/controller/*.cpp #piyushk: commented out because of ModelUpdator duplication
  #src/rl_pursuit/factory/*.cpp
  #src/rl_pursuit/learning/*.cpp
  #src/rl_pursuit/model/*.cpp
  #src/rl_pursuit/planning/*.cpp
)

## The main rl_pursuit library
add_library(rl_pursuit ${SOURCES})
target_link_libraries(rl_pursuit
  json
  gflags
  linear
  blas
  rt
)

## Build all the test executables
## foreach(exec main runClassifier trainClassifier)
##   add_executable(${exec} src/main/${exec}.cpp)
##   target_link_libraries(${exec} rl_pursuit)
## endforeach(exec)
