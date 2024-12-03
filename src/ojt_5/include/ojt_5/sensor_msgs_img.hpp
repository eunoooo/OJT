#ifndef SENSOR_MSGS_IMG_HPP
#define SENSOR_MSGS_IMG_HPP 

#include "ros/ros.h"
#include "iostream"
#include "sensor_msgs/Image.h"
#include "sensor_msgs/CompressedImage.h"
#include "image_transport/image_transport.h"
#include "cv_bridge/cv_bridge.h"
#include "opencv2/opencv.hpp"
#include "vector"


class SensorMsgsImg {
    private:

    ros::NodeHandle n;
    ros::Subscriber sub_row_img;
    ros::Subscriber sub_comp_img;
    ros::Publisher pub_comp_img;

    sensor_msgs::CompressedImage compressed_msg;

    void imageSubCallback(const sensor_msgs::Image::ConstPtr &msg);
    void compSubCallback(const sensor_msgs::CompressedImage::ConstPtr &msg);
    void imagePubCallback(const sensor_msgs::Image::ConstPtr &msg);

    public:
    SensorMsgsImg();
    ~SensorMsgsImg();

    void subscribeImage(const std::string &topic, int queue_size);
    void subscribeCompressedImage(const std::string &topic, int queue_size);
    void advertiseCompressedImage(const std::string &topic, int queue_size);
};

#endif