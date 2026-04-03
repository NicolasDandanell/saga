cmake_minimum_required(VERSION 3.30.0)

# Check if the hringr folder is empty, and run git submodule init if not
file(GLOB HRINGR_FOLDER hringr)
list(LENGTH HRINGR_FOLDER HRINGR_FOLDER_SIZE)
if(HRINGR_FOLDER_SIZE EQUAL 0)
    execute_process(COMMAND "git submodule init")
endif()

# Export own target to Hringr
set(HRINGR_TARGET ${SAGA_TARGET})

# Include the hringr submodule
add_subdirectory(hringr)

# Add the saga.c source file
target_sources(${SAGA_TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/saga.c)

# Add the current directory to the include path
target_include_directories(${SAGA_TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
