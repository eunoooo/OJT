#include "tf_sensor.h"

int main(int argc, char argv){
    ros::init(argc, argv, "transformation_sensor");
   
    TFSensor tfsensor;

    ros::spin();

    return 0;
}