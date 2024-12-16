#include "tf_sensor.h"

TFSensor::TFSensor(){
    sub_scan1 = n.subscribe("/scan1", 10, scan1Callback);
    sub_points = n.subscribe("/camera/depth/points", 10, pointsCallback);

    if(!robot_model.initParam("robot_description")){
        ROS_ERROR("Failed to parse URDF file!!!");
        return -1; 
    }

    front_lidar_point = getPointFromLink(front_laser, "front_laser");
    camera_point = getPointFromLink(camera_link, "camera_link");
}

TFSensor::~TFSeneor(){
}

// todo scan1 이랑 pointcloud 분리하기
void scan1Callback(const sensor_msgs::LaserScan::ConstPtr& msg){
    geometry_msgs::TransformStamped transformStamped, lidar_to_base_point;

    transformStamped = tfBuffer.lookupTransform("base_link", "front_laser", ros::Time(0));
    tf2::doTransform(lidar_point, lidar_to_base_point, transformStamped);

}

/*
void pointsCallback(const sensor_msgs::PointCloud2::ConstPtr& msg){

}
*/


