# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.28.3)

# Include FetchContent for downloading argparse
include(FetchContent)

# If the CMake policies CMP0020 and CMP0071 are not set, set them to "NEW" behavior
if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
    cmake_policy(SET CMP0071 NEW)
endif()

# Set the C++ standard to C++17 for the project.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable automatic handling of Qt MOC, UIC, and RCC for Qt projects (if applicable)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Set the project name and version
project(MedFormatTools VERSION 0.1)

# Fetch argparse
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v3.1
)
FetchContent_MakeAvailable(argparse)

# # Find the local argparse package and include directory for argparse headers
# find_package(argparse REQUIRED)
# message(STATUS "argparse found: ${argparse_FOUND} (version: ${argparse_VERSION})")
# include_directories("/path/to/argparse/v3.1/include")

# Find the ITK package
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
message(STATUS "ITK found: ${ITK_FOUND} (version: ${ITK_VERSION})")

# Include directories for project header files
include_directories(include)

# Define directories for headers and source files in the project
set(HEADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Gather all header (.h) and source (.cxx) files recursively in the respective directories
file(GLOB_RECURSE HEADER_FILES "${HEADERS_DIR}/*.h" SOURCE_FILES "${SOURCE_DIR}/*.cxx")

# Add the executable
add_executable(
    MedFormatTools
    ${HEADER_FILES}
    ${SOURCE_FILES}

)

# Define the MEDFORMATTOOLS_VERSION macro to use in the code
target_compile_definitions(MedFormatTools PRIVATE MEDFORMATTOOLS_VERSION="${PROJECT_VERSION}")

# Link the ITK and argparse libraries to the MedFormatTools executable
target_link_libraries(MedFormatTools PRIVATE argparse::argparse ${ITK_LIBRARIES})