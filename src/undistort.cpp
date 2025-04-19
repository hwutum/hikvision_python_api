#include "undistort.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

ImageUndistorter::ImageUndistorter(const std::string& calibrationFile) {                             // ImageUndistorter类，内外参文件路径作为参数
    cv::FileStorage fs(calibrationFile, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Could not open the calibration file: " << calibrationFile << std::endl;
        return;
    }

    fs["camera_matrix"] >> camera_matrix_;
    fs["distortion_coefficients"] >> dist_coeffs_;
    fs.release();

    std::cout << "Loaded camera matrix: " << camera_matrix_ << std::endl;
    std::cout << "Loaded distortion coefficients: " << dist_coeffs_ << std::endl;
}

void ImageUndistorter::undistort_image(const cv::Mat& src, cv::Mat& dst) {
    if (camera_matrix_.empty() || dist_coeffs_.empty()) {
        std::cerr << "Calibration parameters are not loaded properly." << std::endl;
        return;
    }
    cv::undistort(src, dst, camera_matrix_, dist_coeffs_, camera_matrix_);
}

void ImageUndistorter::add_calibration(const cv::Mat& dst, cv::Mat& calibratedImage) {

    calibratedImage = dst.clone();                                                      // 将dst复制到calibratedImage


    int centerX = calibratedImage.cols / 2;
    int centerY = calibratedImage.rows / 2;                                             // 计算图像的中心点

    cv::line(calibratedImage, cv::Point(centerX - 20, centerY), 
             cv::Point(centerX + 20, centerY), cv::Scalar(0, 0, 255), 2);               // 画线


    cv::line(calibratedImage, cv::Point(centerX, centerY - 20),
             cv::Point(centerX, centerY + 20), cv::Scalar(0, 0, 255), 2);               // 画线
}

cv::Mat ImageUndistorter::get_calibrated_image() const {
    return calibrated_image_;
}
