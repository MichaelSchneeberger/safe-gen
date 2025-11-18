cmake_minimum_required(VERSION 3.28)

project(safegen LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
  safegen
  SOURCE_DIR "../../.."
)

FetchContent_MakeAvailable(safegen)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE safegen)
