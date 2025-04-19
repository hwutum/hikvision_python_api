// device_camera_sy011.cpp

#include "device_camera_sy011.h"
#include <iostream>

DeviceCameraSY011::DeviceCameraSY011() : DeviceCamera() {}

DeviceCameraSY011::~DeviceCameraSY011() {
    close();
}

bool DeviceCameraSY011::init() {
    int nRet = MV_CC_Initialize();
    if (nRet = MV_OK) {
        std::cerr << "Failed to initialize SDK! Error Code: [0x" << std::hex << nRet << "]" << std::endl;
        return false;
    }

    nRet = MV_CC_EnumDevices(MV_USB_DEVICE, &device_list);
    std::cerr << "cam_num:"<<device_list.nDeviceNum<< std::endl;
    if (nRet != MV_OK || device_list.nDeviceNum <= 0) {
        std::cerr << "No devices found! Error Code: [0x" << std::hex << nRet << "]" << std::endl;
        return false;
    }



    return openDevice();
}

bool DeviceCameraSY011::openDevice() {
    MV_CC_DEVICE_INFO* device_info = device_list.pDeviceInfo[0];
    int nRet = MV_CC_CreateHandle(&handle, device_info);
    if (nRet != MV_OK) {
        std::cerr << "Create handle failed! Error Code: [0x" << std::hex << nRet << "]" << std::endl;
        return false;
    }

    nRet = MV_CC_OpenDevice(handle);
    if (nRet != MV_OK) {
        std::cerr << "Open device failed! Error Code: [0x" << std::hex << nRet << "]" << std::endl;
        return false;
    }

    // 设置像素格式为 BGR8
    nRet = MV_CC_SetEnumValue(handle, "PixelFormat", PixelType_Gvsp_BGR8_Packed);
    if (nRet != MV_OK)
    {
        std::cerr << "Failed to set PixelFormat to BGR8! Error Code: [0x" << std::hex << nRet << "]" << std::endl;
        return false;
    }

    // nRet = MV_CC_SetEnumValue(handle, "ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF); // 设置 ExposureAuto
    // if (MV_OK != nRet) {
    //     std::cerr << "Failed to set exposure auto mode: [0x" << std::hex << nRet << "]" << std::endl;
    //   return nRet;
    // }
  
    // // 使用传入的曝光时间设置曝光
    // nRet = MV_CC_SetFloatValue(handle, "ExposureTime", 70000);
    // if (MV_OK != nRet) {
    //     std::cerr << "Failed to set exposure time: [0x" << std::hex << nRet << "]" << std::endl;
    //   return nRet;
    // }

    return true;
}

bool DeviceCameraSY011::set_resolution(int width, int height) {
    int nRet = MV_CC_SetIntValueEx(handle, "Width", width);
    if (nRet != MV_OK) {
        std::cerr << "Failed to set width! Error Code: [0x" << std::hex << nRet << "]" << std::endl;
        return false;
    }

    nRet = MV_CC_SetIntValueEx(handle, "Height", height);
    if (nRet != MV_OK) {
        std::cerr << "Failed to set height! Error Code: [0x" << std::hex << nRet << "]" << std::endl;
        return false;
    }

    return true;
}

bool DeviceCameraSY011::set_exposure_time(int exposure_time) {
    // 暂时注释掉曝光时间设置功能
    int nRet = MV_CC_SetIntValueEx(handle, "ExposureTime", exposure_time);
    if (nRet != MV_OK) {
        std::cerr << "Failed to set exposure time! Error Code: [0x" << std::hex << nRet << "]" << std::endl;
        return false;
    }
    return true;  // 直接返回 true 以绕过曝光时间设置
}


bool DeviceCameraSY011::start_grabbing() {
    int nRet = MV_CC_StartGrabbing(handle);
    if (nRet != MV_OK) {
        std::cerr << "Start grabbing failed! Error Code: [0x" << std::hex << nRet << "]" << std::endl;
        return false;
    }
    return true;
}

void DeviceCameraSY011::stop_grabbing() {
    if (handle) {
        MV_CC_StopGrabbing(handle);
        MV_CC_CloseDevice(handle);
        MV_CC_DestroyHandle(handle);
    }
}

bool DeviceCameraSY011::capture_image(unsigned char* pData, MV_FRAME_OUT_INFO_EX& frameInfo) {
    int nRet = MV_CC_GetOneFrameTimeout(handle, pData, 4024 * 3036, &frameInfo, 1000);
    return nRet == MV_OK;
}

void DeviceCameraSY011::close() {
    if (handle) {
        MV_CC_StopGrabbing(handle);
        MV_CC_CloseDevice(handle);
        MV_CC_DestroyHandle(handle);
    }
    MV_CC_Finalize();
}

void DeviceCameraSY011::image_callback(unsigned char* pData, MV_FRAME_OUT_INFO_EX& frameInfo, void* pUser) {
    DeviceCameraSY011* camera_device = static_cast<DeviceCameraSY011*>(pUser);
    if (camera_device) {
        camera_device->image_callback_handler(pData, frameInfo);
    }
}

void DeviceCameraSY011::image_callback_handler(unsigned char* pData, MV_FRAME_OUT_INFO_EX& frameInfo) {
    std::lock_guard<std::mutex> lock(image_mutex_);
    latest_image_ = cv::Mat(frameInfo.nHeight, frameInfo.nWidth, CV_8UC3, pData);
}