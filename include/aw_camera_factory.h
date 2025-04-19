#ifndef AW_CAMERA_FACTORY_H
#define AW_CAMERA_FACTORY_H

#include "device_camera_base.h"
#include "device_camera_sy011.h"  // 引入SY011相机

class CameraFactory {
public:
    // 枚举相机类型
    enum class CameraType {
        SY011,  // SY011相机
        SIMULATOR  // 模拟器（可扩展）
    };

    // 工厂方法：根据类型创建相机实例
    static std::unique_ptr<DeviceCamera> create_camera(CameraType type) {
        switch (type) {
            case CameraType::SY011:
                return std::make_unique<DeviceCameraSY011>();
            default:
                return nullptr;
        }
    }
};

#endif // AW_CAMERA_FACTORY_H
