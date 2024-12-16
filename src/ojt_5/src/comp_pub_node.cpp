#include "ros/ros.h"
#include "iostream"
#include "sensor_msgs/Image.h"
#include "sensor_msgs/CompressedImage.h"    // 공부하기
#include "image_transport/image_transport.h"
#include "cv_bridge/cv_bridge.h"
#include "opencv2/opencv.hpp"
#include "vector"

class SensorMsgsImg {
    private:

    ros::NodeHandle n;
    ros::Subscriber sub_row_img_and_pub;
    ros::Publisher pub_comp_img;

    sensor_msgs::CompressedImage compressed_msg;

    public:
    SensorMsgsImg() {
        sub_row_img_and_pub = n.subscribe("/camera/color/image_raw", 10, &SensorMsgsImg::imagePubCallback, this);
        pub_comp_img = n.advertise<sensor_msgs::CompressedImage>("/ojt/camera/color/image_raw/compressed", 10);
    }

    void imagePubCallback(const sensor_msgs::Image::ConstPtr &msg){
        if (!msg->data.empty()){
            try {
                cv::Mat image = cv_bridge::toCvCopy(msg, "bgr8")->image;
                std::vector<uchar> compressed_image;
                cv::imencode(".jpg", image, compressed_image);

                compressed_msg.header = msg->header;  
                compressed_msg.format = "jpeg";  
                compressed_msg.data = compressed_image;

                pub_comp_img.publish(compressed_msg);
            } catch (const cv_bridge::Exception& e) {
                ROS_ERROR("cv_bridge exception: %s", e.what());
            } 
        }
    }
};

int main(int argc, char **argv){
    ros::init(argc, argv, "comp_pub_node");

    SensorMsgsImg rowImg;

    ros::spin();
    
    return 0;
}
