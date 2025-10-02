cmake_minimum_required(VERSION 3.28)
project(my_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
# do not fall back to a default cxx standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect sources
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE MODULES CONFIGURE_DEPENDS "src/*.cppm")

message(STATUS "Modules: ${MODULES}")

add_executable(${PROJECT_NAME})

# Add files to the target
target_sources(${PROJECT_NAME}
  PRIVATE
    ${SOURCES}
)

target_sources(${PROJECT_NAME}
  PUBLIC
    FILE_SET all_my_modules TYPE CXX_MODULES FILES
    ${MODULES}
)

