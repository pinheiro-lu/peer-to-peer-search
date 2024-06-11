cmake_minimum_required(VERSION 3.5)
project(ep_distsys)

set(CMAKE_CXX_STANDARD 11)

# Add include directory
include_directories(include)

# Add source files
file(GLOB SOURCES "src/**/*.cpp")

# Add main file
list(APPEND SOURCES "src/main.cpp")

# Add executable
add_executable(ep_distsys ${SOURCES})

# Set up IWYU
find_program(iwyu_path NAMES include-what-you-use iwyu)
if(iwyu_path)
    set_property(TARGET ep_distsys PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
endif()