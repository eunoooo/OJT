#include "ojt_5/sensor_msgs_img.hpp"

int main(int argc, char **argv){
    ros::init(argc, argv, "comp_pub_node");

    SensorMsgsImg rowImg;
    rowImg.advertiseCompressedImage("ojt/camera/color/image_raw/compressed", 10);

    ros::spin();
    
    return 0;
}
