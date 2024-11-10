cmake_minimum_required(VERSION 3.28.3)

# Set the project name
project(MedFormatTools)

# Find the ITK package
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# Add the executable
add_executable(MedFormatTools src/MedFormatTools.cxx)

# Link the ITK libraries
target_link_libraries(MedFormatTools ${ITK_LIBRARIES})