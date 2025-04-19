// device_camera_base.h

#ifndef DEVICE_CAMERA_BASE_H
#define DEVICE_CAMERA_BASE_H

#include <opencv2/opencv.hpp>
#include "MvCameraControl.h"

class DeviceCamera {
public:
    virtual ~DeviceCamera() {}
    
    virtual bool init() = 0;  // Initialization function
    virtual bool set_resolution(int width, int height) = 0;
    // virtual bool set_exposure_time(int exposure_time) = 0;
    virtual bool start_grabbing() = 0;
    virtual void stop_grabbing() = 0;
    
    // Declare capture_image as a virtual function
    virtual bool capture_image(unsigned char* pData, MV_FRAME_OUT_INFO_EX& frameInfo) = 0;

    virtual void close() = 0;

protected:
    void* handle = nullptr;  // Camera handle
    MV_CC_DEVICE_INFO_LIST device_list;  // Device info list
};

#endif // DEVICE_CAMERA_BASE_H
