#ifndef TF_SENSOR
#define TF_SENSOR

#include <ros/ros.h>  
#include <sensor_msgs/LaserScan.h>  
#include <sensor_msgs/PointCloud.h>  
#include <tf2_ros/transform_listener.h>  
#include <geometry_msgs/TransformStamped.h>  
#include <tf/tf.h>  
#include <cmath>  
#include <iostream> 


class TFSensor{
    private:
    ros::NodeHandle n;

    //todo lookuptransform tf 헤더에서 쓰기
    tf2_ros::Buffer tfBuffer;  
    tf2_ros::TransformListener tfListener; 

    ros::Subscriber sub_scan;
    ros::Subscriber sub_points;

    ros::Publisher pub_transform_scan;


    // converCoordinate에 사용되는 변수
    float angle = 0.0f;
    float distance = 0.0f;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    // 회전 변환에 사용되는 변수
    // vertor<tuple<float, float, float>> convertXYZ;
    geometry_msgs::TransformStamped transform_stamped;
    


    void scanCallback(const sensor_msgs::LaserScan::ConstPtr& msg);
    // void pointsCallback(const sensor_msgs::PointCloud2::ConstPtr& msg);

    std::vector<tf::Vector3> convertCoordinate(const sensor_msgs::LaserScan::ConstPtr& msg);
    tf::Vector3 TF(const geometry_msgs::TransformStamped& transform_stamped, tf::Vector3 original_v);


    public:

    TFSensor();
    ~TFSensor();
};

#endif
