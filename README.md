# Hikvision Camera Python Bindings

## Overview

This project provides Python bindings for controlling Hikvision industrial cameras using the MVS SDK. It utilizes `pybind11` to wrap a C++ class (`DeviceCameraSY011`) that interfaces with the camera hardware, allowing easy integration into Python applications. The project also uses OpenCV for potential image processing tasks (though primarily handled in the Python layer in the current examples).

## Features

*   Initialize and connect to the first detected Hikvision camera.
*   Set camera resolution (Width, Height).
*   Start and stop image grabbing.
*   Capture single frames and return them as NumPy arrays (BGR format from C++, convertible in Python).
*   Provides a Python class `HikvisionCamera` (in `cameras/hikvision.py`) mimicking common camera interfaces for easier integration.
*   Includes a command-line script (`cameras/hikvision.py`) for basic testing and image saving.

## Dependencies

*   **C++ Compiler:** A modern C++ compiler supporting C++11 or later (e.g., GCC, Clang).
*   **CMake:** Version 3.12 or higher.
*   **Hikvision MVS SDK:** The Machine Vision Software Development Kit for your camera. You need to download and install this separately from Hikvision.
    *   Download link: [https://www.hikrobotics.com/cn/machinevision/service/download/?module=0](https://www.hikrobotics.com/cn/machinevision/service/download/?module=0)
*   **OpenCV:** Required by the C++ layer.
*   **Python:** Python 3.x development headers and libraries (`python3-dev` or equivalent).
*   **pybind11:** Header-only library for C++/Python bindings. Can be installed via pip (`pip install pybind11`) or system package manager (`pybind11-dev`).
*   **NumPy:** Required by the Python code (`pip install numpy`).
*   **OpenCV Python:** Optional but recommended for display/saving in Python examples (`pip install opencv-python`).

## Build Instructions

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/hwutum/hikvision_python_api.git
    cd hikvision_python_api
    ```

2.  **Configure SDK Path:**
    *   Open the main `CMakeLists.txt` file.
    *   Locate the line `set(HIK_MVS_SDK_PATH "/opt/MVS")`.
    *   **Modify the path** `/opt/MVS` to point to the **root directory** where you installed the Hikvision MVS SDK.

3.  **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```

4.  **Run CMake:**
    ```bash
    cmake ..
    ```
    *   CMake will attempt to find Python, pybind11, OpenCV, and the Hikvision SDK library based on the configured path. Review the output for any errors.

5.  **Compile:**
    ```bash
    make
    ```
    *   This will compile the C++ code and create the Python binding module (e.g., `hikvision_camera.cpython-3x-x86_64-linux-gnu.so`) inside the `build` directory.

## Usage

**Directly using the compiled module (Lower Level):**

```python
import sys
import os
import cv2
import numpy as np

# Add build directory to path
build_dir = os.path.abspath('./build') # Adjust if needed
if build_dir not in sys.path:
    sys.path.insert(0, build_dir)

try:
    import hikvision_camera
except ImportError:
    print(f"Failed to import module. Ensure it's built in {build_dir}")
    sys.exit(1)

cam = hikvision_camera.DeviceCameraSY011()

if not cam.init():
    print("Init failed")
    sys.exit(1)

if not cam.set_resolution(1440, 1080):
    print("Set resolution failed")
    cam.close()
    sys.exit(1)

if not cam.start_grabbing():
    print("Start grabbing failed")
    cam.close()
    sys.exit(1)

print("Grabbing... Press 'q' to quit.")
while True:
    success, frame_np = cam.capture_image()
    if success and isinstance(frame_np, np.ndarray):
        cv2.imshow("Direct Capture", frame_np) # Frame is BGR
    else:
        print("Capture failed")
        # break # Or continue trying

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cam.stop_grabbing()
cam.close()
cv2.destroyAllWindows()
print("Finished.")
```
