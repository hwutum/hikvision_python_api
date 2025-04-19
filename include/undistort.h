#ifndef UNDISTORT_H
#define UNDISTORT_H

#include <iostream>
#include <opencv2/opencv.hpp>

class ImageUndistorter {
public:

    ImageUndistorter(const std::string& calibrationFile);                       //加载标定参数，相机内参，畸变系数（已知）
    ~ImageUndistorter() = default;

    void undistort_image(const cv::Mat& src, cv::Mat& dst);                     //图像去畸变
    void add_calibration(const cv::Mat& dst, cv::Mat& calibratedImage);         //给图片中点添加光标
    cv::Mat get_calibrated_image() const;                                       //获取带有光标的图片

private:
    cv::Mat camera_matrix_;                                                     //相机内参矩阵
    cv::Mat dist_coeffs_;                                                       //畸变系数
    cv::Mat calibrated_image_;                                                  //存储带有光标的图片
};

#endif // UNDISTORT_H
