#include "ojt_6/tf_sensor.h"

TFSensor::TFSensor() : tfListener(tfBuffer) {

    sub_scan = n.subscribe("/scan", 10, &TFSensor::scanCallback, this);
   // sub_points = n.subscribe("/camera/depth/points", 10, pointsCallback);

    // pub_transform_scan1 = n.advertise<sensor_msgs::PointCloud2>("/ojt/transform/scan", 10);
    pub_transform_scan = n.advertise<geometry_msgs::PointStamped>("/ojt/transform/scan", 10);
}

TFSensor::~TFSensor(){
}

// todo scan1 이랑 pointcloud 분리하기
void TFSensor::scanCallback(const sensor_msgs::LaserScan::ConstPtr& msg){
    geometry_msgs::TransformStamped transformStamped;
    geometry_msgs::PointStamped lidar_point, lidar_to_base_point;
    

    transformStamped = tfBuffer.lookupTransform("base_link", "front_laser", ros::Time(0));

    for(int i = 0 ; i < msg->ranges.size() ; i++){
        angle = msg->angle_min + (msg->angle_increment * i);
        distance = msg->ranges[i];

        lidar_point.point.x = distance * cos(angle);
        lidar_point.point.y = distance * sin(angle);
        lidar_point.point.z = 0.0;  // Z는 0으로 설정 (2D 평면)

        lidar_point.header = msg->header;

        // doTransform이 변환행렬을 사용하여 좌표 변환을 수행하는 부분인듯? -> 찾아보기
        // todo 변환행렬 공부
        tf2::doTransform(lidar_point, lidar_to_base_point, transformStamped);

        // todo lidar_to_base_point 를 PointCloud2 type으로 변환하기
 
        //todo publish
        pub_transform_scan.publish(lidar_to_base_point);
    }    
}

/*
void TFSensor::pointsCallback(const sensor_msgs::PointCloud2::ConstPtr& msg){

}
*/


