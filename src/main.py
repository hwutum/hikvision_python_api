import cv2
import numpy as np
import sys
import os
import time

# Add the build directory to the Python path so it can find the module
# Adjust the path './build' if your build directory is elsewhere
build_dir = os.path.abspath('./build')
if build_dir not in sys.path:
    sys.path.insert(0, build_dir)

try:
    import hikvision_camera # Import the compiled module
except ImportError as e:
    print(f"Error importing module: {e}")
    print(f"Ensure '{build_dir}' contains the compiled '.so' file and is in sys.path.")
    sys.exit(1)

def main():
    # Create camera instance from the Python binding
    camera = hikvision_camera.DeviceCameraSY011()

    print("Initializing camera...")
    if not camera.init():
        print("Failed to initialize camera!")
        return

    print("Setting resolution...")
    # Note: Resolution set here is mainly for the C++ side if it uses it internally.
    # The actual image size comes from frameInfo during capture.
    if not camera.set_resolution(1440, 1080):
        print("Failed to set resolution!")
        camera.close()
        return

    # print("Setting exposure...") # Uncomment if using exposure
    # if not camera.set_exposure_time(1000):
    #     print("Failed to set exposure time!")
    #     camera.close()
    #     return

    print("Starting grabbing...")
    if not camera.start_grabbing():
        print("Failed to start grabbing!")
        camera.close()
        return

    print("Camera started successfully. Press 's' to save, 'q' to quit.")

    cv2.namedWindow("Camera Image", cv2.WINDOW_NORMAL)

    save_dir = "pic_python"
    os.makedirs(save_dir, exist_ok=True)

    while True:
        try:
            # Capture image using the Python wrapper method
            success, image_np = camera.capture_image()

            if not success or image_np is None:
                print("Failed to capture image!")
                time.sleep(0.1) # Avoid busy-waiting on error
                continue

            # Ensure it's a valid NumPy array
            if not isinstance(image_np, np.ndarray):
                print("Captured data is not a NumPy array.")
                continue

            # Display the image using OpenCV
            # resize 640 * 480
            image_np = cv2.resize(image_np, (640, 480))
            cv2.imshow("Camera Image", image_np)

            key = cv2.waitKey(1) & 0xFF
            if key == ord('s'):
                filename = os.path.join(save_dir, f"captured_image_{int(time.time())}.png")
                cv2.imwrite(filename, image_np)
                print(f"Image saved as {filename}")
            elif key == ord('q'):
                print("Exiting...")
                break

        except Exception as e:
            print(f"An error occurred during capture/display: {e}")
            break # Exit loop on error

    print("Stopping grabbing...")
    camera.stop_grabbing()
    print("Closing camera...")
    camera.close()
    cv2.destroyAllWindows()
    print("Done.")

if __name__ == "__main__":
    main()