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

    ros::Subscriber sub_row_img_and_pub;
    ros::Publisher pub_comp_img;

    sensor_msgs::CompressedImage compressed_msg;

    void imageSubCallback(const sensor_msgs::Image::ConstPtr &msg);
    void compSubCallback(const sensor_msgs::CompressedImage::ConstPtr &msg);
    void imagePubCallback(const sensor_msgs::Image::ConstPtr &msg);
    
    // todo 생성자에 bool 값 받아서 돌아가게 바꾸기
    public:
    SensorMsgsImg(bool sub_row, bool sub_comp, bool pub_comp) {
        if(sub_row == true){
            sub_row_img = n.subscribe("/camera/color/image_raw", 10, imageSubCallback);

            void imageSubCallback(const sensor_msgs::Image::ConstPtr &msg){
                cv::Mat image = cv_bridge::toCvCopy(msg, "bgr8")->image;
                cv::imshow("Camera Image", image);
                cv::waitKey(1);
            }
        } else if(sub_comp == true){
            sub_comp_img = n.subscribe("/camera/color/image_raw/compressed", 10, compSubCallback);

            void compSubCallback(const sensor_msgs::CompressedImage::ConstPtr &msg){
                std::vector<unsigned char> buffer(msg->data.begin(), msg->data.end());
                cv::Mat image = cv::imdecode(buffer, cv::IMREAD_COLOR);
                cv::imshow("Compressed Image", image);
                cv::waitKey(1);
            }
        } else if(pub_comp == true){
            sub_row_img_and_pub = n.subscribe("/camera/color/image_raw", 10, imagePubCallback);
            pub_comp_img = n.advertise<sensor_msgs::CompressedImage>("/ojt/camera/color/image_raw/compressed", 10);

            void imagePubCallback(const sensor_msgs::Image::ConstPtr &msg){
                cv::Mat image = cv_bridge::toCvCopy(msg, "bgr8")->image;

                std::vector<uchar> compressed_image;
                cv::imencode(".jpg", image, compressed_image);

                compressed_msg.header = msg->header;  
                compressed_msg.format = "jpeg";  
                compressed_msg.data = compressed_image;

                pub_comp_img.publish(compressed_msg);
            }
        } 
    }
};


#endif