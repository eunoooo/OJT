#include "ojt_6/tf_sensor.h"

int main(int argc, char **argv){
    ros::init(argc, argv, "transformation_main");
   
    TFSensor tfsensor;

    ros::spin();

    return 0;
}