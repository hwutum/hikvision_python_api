// device_camera_sy011.h
#ifndef DEVICE_CAMERA_SY011_H
#define DEVICE_CAMERA_SY011_H

#include "device_camera_base.h"
#include <opencv2/opencv.hpp>
#include <mutex>

class DeviceCameraSY011 : public DeviceCamera {
public:
    DeviceCameraSY011();
    virtual ~DeviceCameraSY011();

    bool init() override;
    bool set_resolution(int width, int height);
    bool set_exposure_time(int exposure_time);
    bool start_grabbing();
    void stop_grabbing();
    bool capture_image(unsigned char* pData, MV_FRAME_OUT_INFO_EX& frameInfo);
    void close() override;

    // Add method to get FPS
    float get_fps();

    // Image callback
    static void image_callback(unsigned char* pData, MV_FRAME_OUT_INFO_EX& frameInfo, void* pUser);

private:
    bool openDevice();
    void image_callback_handler(unsigned char* pData, MV_FRAME_OUT_INFO_EX& frameInfo);

    // 加锁保护的成员变量
    std::mutex image_mutex_;
    cv::Mat latest_image_;
};

#endif // DEVICE_CAMERA_SY011_H
