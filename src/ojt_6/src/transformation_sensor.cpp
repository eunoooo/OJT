#include "ojt_6/tf_sensor.h"

TFSensor::TFSensor() : tfListener(tfBuffer) {

    sub_scan = n.subscribe("/scan", 10, &TFSensor::scanCallback, this);
   // sub_points = n.subscribe("/camera/depth/points", 10, pointsCallback);

    pub_transform_scan1 = n.advertise<sensor_msgs::PointCloud2>("/ojt/transform/scan", 10);
    //pub_transform_scan = n.advertise<geometry_msgs::PointStamped>("/ojt/transform/scan", 10);
}

TFSensor::~TFSensor(){
}

// todo scan1 이랑 pointcloud 분리하기
void TFSensor::scanCallback(const sensor_msgs::LaserScan::ConstPtr& msg){
    geometry_msgs::TransformStamped transformStamped;
    geometry_msgs::PointStamped lidar_point, lidar_to_base_point, transformed_point;
    

    transformStamped = tfBuffer.lookupTransform("base_link", "front_laser", ros::Time(0));

    // 극좌표계 -> 데카르트 좌표계
    for(int i = 0 ; i < msg->ranges.size() ; i++){
        angle = msg->angle_min + (msg->angle_increment * i);
        distance = msg->ranges[i];

        x = distance * cos(angle);
        y = distance * sin(angle);
        z = 0.0;  // Z는 0으로 설정 (2D 평면)

        // PointCloud에 포인트 추가
        cloud.push_back(pcl::PointXYZ(x, y, z));
    }

    for (auto& point : cloud.points){
        lidar_point.point.x = point.x;
        lidar_point.point.y = point.y;
        lidar_point.point.z = point.z;
        lidar_point.header.frame_id = scan->header.frame_id;
        lidar_point.header.stamp = ros::Time(0);

        tf2_ros::doTransform(lidar_point, transformed_point, transform);

        // 변환된 좌표를 다시 포인트 클라우드에 저장
        point.x = transformed_point.point.x;
        point.y = transformed_point.point.y;
        point.z = transformed_point.point.z;
    }

    // PCL 포인트 클라우드 데이터를 sensor_msgs::PointCloud2 형식으로 변환
    pcl::toROSMsg(cloud, lidar_pointcloud);

    lidar_pointcloud.header.stamp = ros::Time::now();
    lidar_pointcloud.header.frame_id = "base_link"; 


    // doTransform이 변환행렬을 사용하여 좌표 변환을 수행하는 부분인듯? -> 찾아보기
    // todo 변환행렬 공부
    //tf2::doTransform(lidar_point, lidar_to_base_point, transformStamped);  

    //todo publish
    pub_transform_scan.publish(lidar_pointcloud);
}



/*
void TFSensor::pointsCallback(const sensor_msgs::PointCloud2::ConstPtr& msg){

}
*/


