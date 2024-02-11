cmake_minimum_required(VERSION 3.16)

project(
  afaminisat
  VERSION 0.1.0
  DESCRIPTION "MiniSAT adjusted and optimized to solve AFA emptiness"
  LANGUAGES CXX
)

# find_package(LibLZMA REQUIRED)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DMY_DEBUG")

option(OPTIONAL_CLAUSES "" ON)

set(FLAGS "")

if (OPTIONAL_CLAUSES)
  set(FLAGS "${FLAGS} -DOPTIONAL_CLAUSES")
endif()

set(CELL_CONTAINER "${CELL_CONTAINER}")
if (CELL_CONTAINER STREQUAL "DFS")
  set(FLAGS "${FLAGS} -DCELL_CONTAINER_DFS")
elseif (CELL_CONTAINER STREQUAL "BFS")
  set(FLAGS "${FLAGS} -DCELL_CONTAINER_BFS")
elseif (CELL_CONTAINER STREQUAL "SET")
  set(FLAGS "${FLAGS} -DCELL_CONTAINER_SET")
elseif (CELL_CONTAINER STREQUAL "")
  set(FLAGS "${FLAGS} -DCELL_CONTAINER_SET")
else()
  message(FATAL_ERROR "Unknown CELL_CONTAINER ${CELL_CONTAINER}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${FLAGS}")

set(DEPS "")
set(DEPS "${DEPS};Antichain.C")

find_library(CryptoMiniSat cryptominisat5 REQUIRED)
find_path(CryptoMiniSatInclude cryptominisat5 REQUIRED)

add_executable(
  cryptominisat-antichain
  ${DEPS}
)
set_property(TARGET cryptominisat-antichain PROPERTY CXX_STANDARD 20)
target_include_directories(cryptominisat-antichain PRIVATE "${CryptoMiniSatInclude}")
target_link_libraries(cryptominisat-antichain PRIVATE "${CryptoMiniSat}")
