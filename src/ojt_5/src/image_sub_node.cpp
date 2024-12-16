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
    sensor_msgs::CompressedImage compressed_msg;

public:
    SensorMsgsImg() {
        sub_row_img = n.subscribe("/camera/color/image_raw", 10, &SensorMsgsImg::imageSubCallback, this);
    }

    void imageSubCallback(const sensor_msgs::Image::ConstPtr &msg){
        if(!msg->data.empty()) {
            try {
                cv::Mat image = cv_bridge::toCvCopy(msg, "bgr8")->image;
                if(!image.empty()) {
                    cv::imshow("Camera Image", image);  // 이미지를 창에 표시
                    cv::waitKey(1);  // 이미지를 화면에 업데이트
                }
            } catch (const cv_bridge::Exception& e) {
                ROS_ERROR("cv_bridge exception: %s", e.what());
            } 
        }
    }
};

int main(int argc, char **argv){
    ros::init(argc, argv, "image_sub_node"); 

    SensorMsgsImg rowImg;

    ros::spin();  

    return 0;
}