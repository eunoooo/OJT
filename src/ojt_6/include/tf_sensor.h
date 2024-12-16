#ifndef TF_SENSOR
#define TF_SENSOR

#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/PointCloud2.h>
#include <tf2_ros/transform_listener.h>
#include <geometry_msgs/TransformStamped.h>
#include <cmath>
#include <iostream>


class TFSensor {
    private:
    ros::NodeHandle n;

    //todo tf listener를 사용하여 /tf_static 토픽을 구독해서 각 link 간의 관계를 받아오고,
    //todo callback 함수에서 transformation을 적용한 메세지를
    //todo sensor_msgs/PointCloud2 type으로 publish
    tf2_ros::Buffer tfBuffer;
    tf2_ros::TransformListener tfListener(tfBuffer);
    
    ros::Subscriber sub_scan1;
    ros::Subscriber sub_points;

    ros::Publisher pub_transform_scan1;


    float angle = 0;
    float distance = 0;

    public:
    TFSensor();
    ~TFSensor();
};

#endif