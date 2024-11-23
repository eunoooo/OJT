#include "ros/ros.h"
#include "std_msgs/String.h"

ros::Publisher chatter_pub_2; //퍼블리셔를 전역으로 선언

void chatterCallback(const std_msgs::String::ConstPtr &msg)
{
    ROS_INFO("I heard: [%s]", msg->data.c_str());

    chatter_pub_2.publish(msg);

}

int main(int argc, char **argv)
{
  
  ros::init(argc, argv, "test_sub");

  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe("chatter_pub", 10, chatterCallback);
  chatter_pub_2 = n.advertise<std_msgs::String>("chatter_pub_2_topic", 10);

  ros::spin();

  return 0;
}