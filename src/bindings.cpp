#include <pybind11/pybind11.h>
#include <pybind11/stl.h> // Needed for automatic type conversions (e.g., std::string)
#include <pybind11/numpy.h> // Needed for returning images as NumPy arrays
#include <pybind11/stl_bind.h> // Include for vector bindings if needed elsewhere
#include "device_camera_sy011.h" // Include your camera header
#include "MvCameraControl.h"   // Include Hikvision SDK header
#include <stdexcept> // For exceptions
#include <vector> // Include vector

namespace py = pybind11;

// Wrapper class to manage buffer allocation for capture_image
class PyDeviceCameraSY011 : public DeviceCameraSY011 {
public:
    using DeviceCameraSY011::DeviceCameraSY011; // Inherit constructors

    // Wrapper for capture_image that returns a NumPy array
    py::tuple capture_image_py() {
        // Estimate buffer size (adjust if necessary, e.g., based on current resolution)
        // Consider getting width/height after setting resolution if possible
        int buffer_size = 1440 * 1080 * 3; // Max resolution buffer * 3 channels (BGR)
        std::vector<unsigned char> buffer(buffer_size);
        MV_FRAME_OUT_INFO_EX frameInfo = {0};

        bool success = capture_image(buffer.data(), frameInfo);

        if (!success) {
            // Return None or raise an exception on failure
             return py::make_tuple(false, py::none());
            // Or: throw std::runtime_error("Failed to capture image");
        }

        // Check if frameInfo contains valid dimensions
        if (frameInfo.nWidth <= 0 || frameInfo.nHeight <= 0) {
             return py::make_tuple(false, py::none());
            // Or: throw std::runtime_error("Captured frame has invalid dimensions");
        }

        // Explicitly define shape and strides using std::vector
        std::vector<py::ssize_t> shape = { (py::ssize_t)frameInfo.nHeight, (py::ssize_t)frameInfo.nWidth, 3 };
        std::vector<py::ssize_t> strides = { (py::ssize_t)frameInfo.nWidth * 3, 3, 1 };

        // Create a NumPy array from the captured data
        // Assuming BGR8 format is set in C++
        py::array_t<unsigned char> result = py::array_t<unsigned char>(
            shape,           // Shape (rows, cols, channels)
            strides,         // Strides (bytes per row, bytes per pixel, bytes per channel)
            buffer.data(),   // Pointer to data
            py::cast(*this)  // Keep the C++ object alive
        );

        // Important: Copy the data, as the buffer vector will go out of scope.
        // If performance is critical, more advanced buffer management might be needed.
        return py::make_tuple(true, result.attr("copy")());
    }
};


PYBIND11_MODULE(hikvision_camera, m) {
    m.doc() = "Python bindings for Hikvision Camera Control"; // Optional module docstring

    py::class_<PyDeviceCameraSY011>(m, "DeviceCameraSY011")
        .def(py::init<>()) // Bind constructor
        .def("init", &PyDeviceCameraSY011::init, "Initialize the camera SDK and find devices")
        .def("set_resolution", &PyDeviceCameraSY011::set_resolution, "Set camera resolution", py::arg("width"), py::arg("height"))
        .def("set_exposure_time", &PyDeviceCameraSY011::set_exposure_time, "Set camera exposure time", py::arg("exposure_time")) // Uncommented as per suggestion
        .def("start_grabbing", &PyDeviceCameraSY011::start_grabbing, "Start image grabbing")
        .def("stop_grabbing", &PyDeviceCameraSY011::stop_grabbing, "Stop image grabbing")
        .def("capture_image", &PyDeviceCameraSY011::capture_image_py, "Capture an image and return as NumPy array (success_flag, image_array)")
        .def("close", &PyDeviceCameraSY011::close, "Close the device and release SDK resources")
        .def("get_fps", &PyDeviceCameraSY011::get_fps, "Get the current frame rate reported by the camera (ResultingFrameRate/AcquisitionFrameRate)"); // Added as per suggestion

    // You might need to bind constants or enums from MvCameraControl.h if your Python code needs them
    // Example: m.attr("MV_OK") = py::int_(MV_OK);
}