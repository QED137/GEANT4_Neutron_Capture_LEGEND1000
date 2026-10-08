cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(neutron CXX)

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(neu main.cc ${sources} ${headers})
target_link_libraries(neu ${Geant4_LIBRARIES})

# copy macros into the build directory if building out-of-source
if(NOT "${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
  file(COPY ${PROJECT_SOURCE_DIR}/macros/ DESTINATION ${PROJECT_BINARY_DIR}/macros/)
  file(COPY ${PROJECT_SOURCE_DIR}/scripts/ DESTINATION ${PROJECT_BINARY_DIR}/scripts/)
endif()
