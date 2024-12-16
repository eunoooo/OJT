#ifndef TF_SENSOR
#define TF_SENSOR

#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/PointCloud2.h>
#include <tf2_ros/transform_listener.h>
#include <geometry_msgs/PointStamped.h>  // PointStamped 포함
#include <geometry_msgs/TransformStamped.h>
#include <pcl_ros/point_cloud.h>
#include <cmath>
#include <iostream>


class TFSensor {
    private:
    ros::NodeHandle n;

    //todo tf listener를 사용하여 /tf_static 토픽을 구독해서 각 link 간의 관계를 받아오고,
    //todo callback 함수에서 transformation을 적용한 메세지를
    //todo sensor_msgs/PointCloud2 type으로 publish

    tf2_ros::Buffer tfBuffer;  
    tf2_ros::TransformListener tfListener; 

    ros::Subscriber sub_scan;
    ros::Subscriber sub_points;

    ros::Publisher pub_transform_scan;


    float angle = 0;
    float distance = 0;

    float x, y, z;

    // 변환된 포인트 클라우드를 담을 객체
    pcl::PointCloud<pcl::PointXYZ> cloud;

    sensor_msgs::PointCloud2 lidar_pointcloud;

    void scanCallback(const sensor_msgs::LaserScan::ConstPtr& msg);
    // void pointsCallback(const sensor_msgs::PointCloud2::ConstPtr& msg);

    public:
    TFSensor();
    ~TFSensor();
};

#endif
