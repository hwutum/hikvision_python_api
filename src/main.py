import cv2
import numpy as np
import sys
import os
import time

# Add the build directory to the Python path so it can find the module
# Adjust the path './build' if your build directory is elsewhere
# Go up one level from src, then into build
build_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'build'))
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
    if not camera.set_resolution(1440, 1080):
        print("Failed to set resolution!")
        camera.close()
        return

    # print("Setting exposure...") # Uncomment if using exposure
    # if not camera.set_exposure_time(100): # Example exposure
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

    # --- FPS Calculation/Display Variables ---
    frame_count_calc = 0
    fps_calc = 0.0
    start_time_calc = time.perf_counter()
    fps_update_interval = 1.0 # Update FPS display every 1 second

    last_sdk_fps_time = 0
    sdk_fps = -1.0 # Initialize SDK FPS
    sdk_fps_update_interval = 2.0 # Update SDK FPS less frequently
    # --- End FPS Variables ---

    while True:
        loop_start_time = time.perf_counter()
        try:
            # Capture image using the Python wrapper method
            success, image_np = camera.capture_image()

            if not success or image_np is None:
                # print("Failed to capture image!") # Reduce console spam
                time.sleep(0.05) # Avoid busy-waiting on error
                continue

            # Ensure it's a valid NumPy array
            if not isinstance(image_np, np.ndarray):
                print("Captured data is not a NumPy array.")
                continue

            # --- Calculated FPS ---
            frame_count_calc += 1
            current_time = time.perf_counter()
            elapsed_time_calc = current_time - start_time_calc
            if elapsed_time_calc >= fps_update_interval:
                fps_calc = frame_count_calc / elapsed_time_calc
                frame_count_calc = 0
                start_time_calc = current_time
            # --- End Calculated FPS ---

            # --- Get SDK FPS periodically ---
            if current_time - last_sdk_fps_time >= sdk_fps_update_interval:
                sdk_fps = camera.get_fps() # Call the new C++ binding
                last_sdk_fps_time = current_time
            # --- End SDK FPS ---


            # Display the image using OpenCV
            # resize 640 * 480
            display_image = cv2.resize(image_np, (640, 480))

            # --- Display FPS on image ---
            calc_fps_text = f"Calc FPS: {fps_calc:.2f}"
            sdk_fps_text = f"SDK FPS: {sdk_fps:.2f}" if sdk_fps >= 0 else "SDK FPS: N/A"
            cv2.putText(display_image, calc_fps_text, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
            cv2.putText(display_image, sdk_fps_text, (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 255), 2) # Yellow
            # --- End Display FPS ---

            cv2.imshow("Camera Image", display_image)

            key = cv2.waitKey(1) & 0xFF
            if key == ord('s'):
                filename = os.path.join(save_dir, f"captured_image_{int(time.time())}.png")
                # Save the original high-res image
                cv2.imwrite(filename, image_np)
                print(f"Image saved as {filename}")
            elif key == ord('q'):
                print("Exiting...")
                break

        except Exception as e:
            print(f"An error occurred during capture/display: {e}")
            break # Exit loop on error

        # Optional: Print loop time to diagnose bottlenecks
        # loop_time_ms = (time.perf_counter() - loop_start_time) * 1000
        # print(f"Loop time: {loop_time_ms:.2f} ms")


    print("Stopping grabbing...")
    camera.stop_grabbing()
    print("Closing camera...")
    camera.close()
    cv2.destroyAllWindows()
    print("Done.")

if __name__ == "__main__":
    main()