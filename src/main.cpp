#include <iostream>
#include <opencv2/opencv.hpp>
#include "device_camera_sy011.h"
#include "undistort.h"  // 引入去畸变头文件
#include <filesystem>

int main() {
    // 创建相机实例
    DeviceCameraSY011 camera;

    // 初始化相机
    if (!camera.init()) {
        std::cerr << "Failed to initialize the camera!" << std::endl;
        return -1;
    }

    // 设置分辨率为 1440x1080
    if (!camera.set_resolution(1440, 1080)) {
        std::cerr << "Failed to set resolution!" << std::endl;
        return -1;
    }

    // 创建去畸变对象，传入标定文件路径
    ImageUndistorter undistorter("calibration_parameters.yml");

    // 设置曝光时间为 1000 微秒
    // if (!camera.set_exposure_time(100)) {
    //     std::cerr << "Failed to set exposure time!" << std::endl;
    //     return -1;
    // }

    // 开始抓取图像
    if (!camera.start_grabbing()) {
        std::cerr << "Failed to start grabbing images!" << std::endl;
        return -1;
    }

    std::cout << "Started grabbing images successfully!" << std::endl;

    // 创建 OpenCV 窗口
    cv::namedWindow("Camera Image", cv::WINDOW_NORMAL);

    // 图像缓冲区
    unsigned char* pData = new unsigned char[1440 * 1080 * 3];  // 修改为彩色图像缓冲区
    MV_FRAME_OUT_INFO_EX frameInfo = {0};

    // 捕获图像并显示
    while (true) {
        if (!camera.capture_image(pData, frameInfo)) {
            std::cerr << "Failed to capture image!" << std::endl;
            break;
        }

        // 将捕获的数据转换为 OpenCV Mat 对象，改为支持彩色图像
        cv::Mat image(frameInfo.nHeight, frameInfo.nWidth, CV_8UC3, pData);  // 使用彩色图像

        // 进行去畸变处理
        cv::Mat undistorted_image;
        undistorter.undistort_image(image, undistorted_image);  // Apply undistortion

        // 在图像中间添加十字
        undistorter.add_calibration(undistorted_image, undistorted_image);  // Add cross at the center

        // 显示图像
        cv::imshow("Camera Image", image);

        // 按 's' 键保存当前图像
        if (cv::waitKey(1) == 's') {
            // 构造保存文件名，包含时间戳
            std::string filename = "pic/captured_image_" + std::to_string(cv::getTickCount()) + ".png";
            cv::imwrite(filename, image);  // Save the captured image
            std::cout << "Image saved as " << filename << std::endl;
        }
        // 按 'q' 键退出
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    // 停止抓取图像
    camera.stop_grabbing();
    camera.close();

    // 释放内存
    delete[] pData;

    cv::destroyAllWindows();

    return 0;
}
