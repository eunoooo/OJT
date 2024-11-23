#include "ros/ros.h"
#include "std_msgs/String.h"
#include "nav_msgs/Odometry.h"
#include "tf2_msgs/TFMessage.h"
#include "sensor_msgs/CompressedImage.h"

ros::Publisher p1;
ros::Publisher p2;
ros::Publisher p3;
ros::Publisher p4;
ros::Publisher p5;


void servinggoTopicCallback1(const sensor_msgs::CompressedImage::ConstPtr &msg)
{
    p1.publish(msg);
}
void servinggoTopicCallback2(const std_msgs::String::ConstPtr &msg)
{
    p2.publish(msg);
}
void servinggoTopicCallback3(const nav_msgs::Odometry::ConstPtr &msg)
{
    p3.publish(msg);
}
void servinggoTopicCallback4(const std_msgs::String::ConstPtr &msg)
{
    p4.publish(msg);
}
void servinggoTopicCallback5(const tf2_msgs::TFMessage::ConstPtr &msg)
{
    p5.publish(msg);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "servinggo_topic_sub");

    ros::NodeHandle n;
    
    ros::Subscriber s1 = n.subscribe("/cona/Planner_v2/local_map", 10, servinggoTopicCallback1);
    ros::Subscriber s2 = n.subscribe("/cona/log", 10, servinggoTopicCallback2);
    ros::Subscriber s3 = n.subscribe("/odom", 10, servinggoTopicCallback3);
    ros::Subscriber s4 = n.subscribe("/servinggo/state", 10, servinggoTopicCallback4);
    ros::Subscriber s5 = n.subscribe("/tf", 10, servinggoTopicCallback5);

    p1 = n.advertise<sensor_msgs::CompressedImage>("/ojt/cona/Planner_v2/local_map", 10);
    p2 = n.advertise<std_msgs::String>("/ojt/cona/log", 10);
    p3 = n.advertise<nav_msgs::Odometry>("/ojt/odom", 10);
    p4 = n.advertise<std_msgs::String>("/ojt/servinggo/state", 10);
    p5 = n.advertise<tf2_msgs::TFMessage>("/ojt/tf", 10);
    
    ros::spin();

    return 0;
}