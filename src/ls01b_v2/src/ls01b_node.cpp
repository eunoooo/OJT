/*******************************************************
@company: Copyright (C) 2018, Leishen Intelligent System
@product: LS01B
@filename: ls01b_node.cpp
@brief:
@version:       date:       author:     comments:
@v2.0           24-02-16    yn          recovery
*******************************************************/
#include "ls01b_v2/ls01b_node.h"
#include "ls01b_v2/ls01b.h"
#include <signal.h>
// #include "ros/ros.h"

namespace ls {

LS01B_Node::LS01B_Node() : ls01b_(nullptr)
{
  initParam();
  pub_ = n_.advertise<sensor_msgs::LaserScan>(scan_topic_, 1000);
  timer_ = n_.createTimer(ros::Duration(0.1), &LS01B_Node::publishScan, this);

  init();
}

LS01B_Node::~LS01B_Node()
{
  close();
}

void LS01B_Node::initParam()
{
  std::string scan_topic = "/scan2";
  std::string frame_id = "laser2";
  std::string port = "/dev/rear_laser";
  ros::NodeHandle nh("~");
  nh.param("scan_topic", scan_topic_, scan_topic);
  nh.param("frame_id", frame_id_, frame_id);
  nh.param("serial_port", serial_port_, port);
  nh.param("baud_rate", baud_rate_, 460800);
  nh.param("angle_resolution", angle_resolution_, 1.0);
  nh.param("max_range", max_range_, 25.0);
  nh.param("angle_disable_min_0", angle_disable_min_0, -1.0);
  nh.param("angle_disable_max_0", angle_disable_max_0, -1.0);
  nh.param("angle_disable_min_1", angle_disable_min_1, -1.0);
  nh.param("angle_disable_max_1", angle_disable_max_1, -1.0);
  nh.param("angle_disable_min_2", angle_disable_min_2, -1.0);
  nh.param("angle_disable_max_2", angle_disable_max_2, -1.0);
  nh.param("angle_disable_min_3", angle_disable_min_3, -1.0);
  nh.param("angle_disable_max_3", angle_disable_max_3, -1.0);
  nh.param("angle_disable_min_4", angle_disable_min_4, -1.0);
  nh.param("angle_disable_max_4", angle_disable_max_4, -1.0);
  nh.param("robot_radius", robot_radius_, 0.2);
  nh.param("center_x", center_x_, 0.0);
  nh.param("center_y", center_y_, 0.0);
}

int LS01B_Node::init()
{
  ROS_INFO("LS01B_Node init");

  close();
  ls01b_ = LS01B::instance(serial_port_, baud_rate_, angle_resolution_);
  return run();
}

void LS01B_Node::close()
{
  if(ls01b_ != nullptr)
  {
    ls01b_->stopScan();
    ls01b_->close();
    delete ls01b_;
    ls01b_ = nullptr;
  }
}

int LS01B_Node::run()
{
  //ls01b_->stopScan();
  // sleep(1);
  ls01b_->setResolution(angle_resolution_);
  ls01b_->setMotorSpeed(600);
 
  // sleep(1);
  ls01b_->switchData(false);

  // sleep(1);
  ls01b_->startScan();

  // sleep(1);
  return ls01b_->setScanMode(true);
}

void LS01B_Node::publishScan(const ros::TimerEvent &)
{
  bool is_health = ls01b_->isHealth();
  if (!is_health)
  {
    ROS_WARN("Try to reset");
    ls01b_->stopScan();
    ls01b_->resetHealth();
    if(init() < 0) sleep(1.0);
    return;
  }
  std::vector<ScanPoint> points;
  ros::Time start_time;
  float scan_time;
  ls01b_->getScan(points, start_time, scan_time);
  int count = points.size();
  if (count <= 0)
  {
    return;
  }
  sensor_msgs::LaserScan msg;
  msg.header.frame_id = frame_id_;
  msg.header.stamp = start_time;
  msg.angle_min = 0.0;
  msg.angle_max = 2*M_PI;
  msg.angle_increment = (msg.angle_max - msg.angle_min) / count;
  msg.range_min = 0.01;
  msg.range_max = max_range_;
  msg.ranges.resize(count);
  msg.intensities.resize(count);
  msg.scan_time = scan_time;
  msg.time_increment = scan_time / (double)(count - 1);

  for (int i = count - 1; i >= 0; i--)
  {
    if (
        ((i >= (angle_disable_min_0 * count/360)) && (i < (angle_disable_max_0 * count/360)))
        || ((i >= (angle_disable_min_1 * count/360)) && (i < (angle_disable_max_1 * count/360)))
        || ((i >= (angle_disable_min_2 * count/360)) && (i < (angle_disable_max_2 * count/360)))
        || ((i >= (angle_disable_min_3 * count/360)) && (i < (angle_disable_max_3 * count/360)))
        || ((i >= (angle_disable_min_4 * count/360)) && (i < (angle_disable_max_4 * count/360)))
        )
    {
      msg.ranges[i] = std::numeric_limits<float>::infinity();
      msg.intensities[i] = 0;
    }
    else if (points[count - i - 1].range == 0.0)
    {
      msg.ranges[i] = std::numeric_limits<float>::infinity();
      msg.intensities[i] = 0;
    }
    else
    {
      if(max_range_ < points[count - i - 1].range){
        msg.ranges[i] = std::numeric_limits<float>::infinity();
        msg.intensities[i] = 0;
      }
      else{
        msg.ranges[i] = points[count - i - 1].range;
        msg.intensities[i] = points[count-i-1].intensity;
      }
    }
    double point_dist = msg.ranges[i];
    if(point_dist < 1.0 && point_dist > 0.06)
    {
      double x = point_dist*cos(i*angle_resolution_*M_PI/180);
      double y = point_dist*sin(i*angle_resolution_*M_PI/180);

      double dist2center = sqrt((y-center_y_)*(y-center_y_) + (x-center_x_)*(x-center_x_));
      if (dist2center < robot_radius_)
      {
        msg.ranges[i] = std::numeric_limits<float>::infinity();
      }
    }
  }
  pub_.publish(msg);

}
}

void handleSig(int signo)
{
  printf("handleSig\n");
  ros::shutdown();
  exit(0);
}

int main(int argv, char **argc)
{
  signal(SIGINT, handleSig);
  signal(SIGTERM, handleSig);
  ros::init(argv, argc, "ls01b_v2");

  ls::LS01B_Node ls01b_node;

  ros::spin();
  return 0;
}
