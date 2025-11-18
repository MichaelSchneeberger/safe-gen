cmake_minimum_required(VERSION 3.28)
project(safegen LANGUAGES CXX)

add_library(safegen)
target_sources(safegen
    PUBLIC
        FILE_SET CXX_MODULES FILES
            src/safegen.cppm
)

target_compile_features(safegen PUBLIC cxx_std_26)

install(TARGETS safegen
    FILE_SET CXX_MODULES DESTINATION share/safegen/modules
)
