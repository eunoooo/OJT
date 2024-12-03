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
    ros::Subscriber sub_comp_img;

    public:
    SensorMsgsImg() {
        sub_comp_img = n.subscribe("/camera/color/image_raw/compressed", 10, &SensorMsgsImg::compSubCallback, this);
    }

    void compSubCallback(const sensor_msgs::CompressedImage::ConstPtr &msg){
        if (!msg->data.empty()) {
            try {
                std::vector<unsigned char> buffer(msg->data.begin(), msg->data.end());
                cv::Mat image = cv::imdecode(buffer, cv::IMREAD_COLOR);
                cv::imshow("Compressed Image", image);
                cv::waitKey(1);
            } catch (const cv_bridge::Exception& e) {
                ROS_ERROR("cv_bridge exception: %s", e.what());
            } 
        }
    }

};

int main(int argc, char **argv){
    ros::init(argc, argv, "comp_sub_node");

    SensorMsgsImg compImg;

    ros::spin();

    return 0;
}