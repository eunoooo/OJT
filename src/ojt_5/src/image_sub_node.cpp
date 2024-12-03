#include "ojt_5/sensor_msgs_img.hpp"

int main(int argc, char **argv){
    ros::init(argc, argv, "image_sub_node");

    SensorMsgsImg rowImg;
    rowImg.subscribeImage("/camera/color/image_raw", 10);

    ros::spin();

    return 0;
}