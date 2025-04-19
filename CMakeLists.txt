cmake_minimum_required(VERSION 3.10)
project(HikvisionDemo)

# 源文件配置
set(src_files
    "src/device_camera_sy011.cpp"
    "src/main.cpp"
    "src/undistort.cpp"
)

# 第三方库路径配置
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
set(HAIKANG_DIR ${THIRD_PARTY_DIR}/hikvision)
set(HAIKANG_LIB_DIR ${HAIKANG_DIR}/lib)

# 查找依赖包
find_package(OpenCV REQUIRED)

# 创建可执行文件
add_executable(camera_test ${src_files})

# 包含目录配置
target_include_directories(camera_test
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${HAIKANG_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

# 链接库配置
target_link_libraries(camera_test
    ${HAIKANG_LIB_DIR}/libMvCameraControl.so
    ${OpenCV_LIBS}
    pthread
)

install(TARGETS camera_test
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY config DESTINATION .)