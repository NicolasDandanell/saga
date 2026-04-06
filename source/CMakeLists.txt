cmake_minimum_required(VERSION 3.30.0)

# Export own target to Hringr
set(HRINGR_TARGET ${SAGA_TARGET})

# Check if the hringr folder is empty
if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/hringr/source)
    # Include the hringr submodule
    add_subdirectory(hringr)
else()
    # The Hringr folder is empty. This is most likely because the Hringr submodule has not been initialized
    message(FATAL_ERROR "The Hringr submodule of Saga has not been initialized!\nPlease run \"git submodule update --init --recursive\" to fix this")
endif()

# Add the saga.c source file
target_sources(${SAGA_TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/saga.c)

# Add the current directory to the include path
target_include_directories(${SAGA_TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
