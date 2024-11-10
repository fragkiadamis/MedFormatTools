cmake_minimum_required(VERSION 3.28.3)

# Set the project name
project(MedFormatTools)

find_package(argparse REQUIRED)
message(STATUS "argparse found: ${argparse_FOUND} (version: ${argparse_VERSION})")
include_directories("/home/fragkiadamis/Applications/argparse/v3.1/include")

# Find the ITK package
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
message(STATUS "ITK found: ${ITK_FOUND} (version: ${ITK_VERSION})")

# Add the executable
add_executable(MedFormatTools src/MedFormatTools.cxx)

# Add the argparse include directory
# target_include_directories(MedFormatTools PRIVATE "/home/fragkiadamis/Applications/argparse/v3.1/include/argparse/argparse.hpp")

# Link the ITK libraries
target_link_libraries(MedFormatTools ${ITK_LIBRARIES})