cmake_minimum_required(VERSION 3.30.0)

project(
    Saga
    VERSION 1.0.0
    DESCRIPTION "A dictionary based logging library"
)

# TEST TARGET
add_library(saga STATIC)

set(SAGA_TARGET saga)

# Add the source subdirectory
add_subdirectory(source)
