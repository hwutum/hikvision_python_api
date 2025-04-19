cmake_minimum_required(VERSION 3.12)
project(hikvision_camera_wrapper LANGUAGES CXX)

# --- User Configuration ---
# Set path to Hikvision MVS SDK (Adjust this path!)
set(HIK_MVS_SDK_PATH "/opt/MVS") # Or wherever your SDK is installed
# --- End User Configuration ---

# Find Python and pybind11
find_package(PythonLibs REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# Find OpenCV (required by your C++ code)
find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV not found!")
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include # Your project's include dir
    ${pybind11_INCLUDE_DIRS}
    ${PYTHON_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${HIK_MVS_SDK_PATH}/include # Hikvision SDK include dir
)

# Hikvision SDK Library
# Adjust library name and path if necessary (e.g., MvCameraControl for x64)
find_library(HIK_MVS_LIB NAMES MvCameraControl HIK_MVS_LIB PATHS ${HIK_MVS_SDK_PATH}/lib/64 NO_DEFAULT_PATH)
if(NOT HIK_MVS_LIB)
    message(FATAL_ERROR "Hikvision MVS library (libMvCameraControl.so) not found in ${HIK_MVS_SDK_PATH}/lib/64")
endif()
message(STATUS "Found Hikvision MVS Lib: ${HIK_MVS_LIB}")


# Add your C++ source files
add_library(hikvision_camera_cpp SHARED
    src/device_camera_sy011.cpp
    # Add other .cpp files if DeviceCameraSY011 depends on them
    # src/device_camera_base.cpp # If base class has implementation
)

# Link C++ library against dependencies
target_link_libraries(hikvision_camera_cpp PRIVATE
    ${OpenCV_LIBS}
    ${HIK_MVS_LIB}
    pthread # Often required by the SDK
)

# Create the Python module
pybind11_add_module(hikvision_camera # Python module name
    src/bindings.cpp # Your wrapper file
)

# Link Python module against your C++ library and dependencies
target_link_libraries(hikvision_camera PRIVATE
    hikvision_camera_cpp # Link against your compiled C++ code
    ${OpenCV_LIBS}
    ${HIK_MVS_LIB}
    pthread
)
