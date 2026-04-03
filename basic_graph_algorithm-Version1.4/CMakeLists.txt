cmake_minimum_required(VERSION 3.16)

project(basic_graph_algorithm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(graph_algorithms
  src/graph_algorithm.cpp
  src/matrix.cpp
)

target_include_directories(graph_algorithms
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(graph_algorithm_cli
  src/main.cpp
)

target_link_libraries(graph_algorithm_cli
  PRIVATE
    graph_algorithms
)
