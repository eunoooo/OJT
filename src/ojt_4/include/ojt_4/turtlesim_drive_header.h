#ifndef OJT_4_TURTLESIM_DRIVE_HEADER         // 헤더 파일 가드
#define OJT_4_TURTLESIM_DRIVE_HEADER      // 헤더 파일 정의 

#include "ros/ros.h"
#include "std_msgs/Bool.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Pose.h"     //<>랑 "" 차이 찾아보기 -> ""는 현재디렉터리에서 찾음 <>는 시스템경로
#include "turtlesim/Kill.h"
#include "turtlesim/Spawn.h"
#include "std_srvs/Empty.h"
#include <iostream>
#include <math.h>

class TurtlesimDrive {
    private:
    geometry_msgs::Twist twist;
    double current_pose_x = 0.0;
    double current_pose_y = 0.0;
    double current_theta = 0.0;
    double target_pose_x = 0.0;
    double target_pose_y = 0.0;
 
    void driveTurtlesim();                    // 전체 주행 로직 함수

    void driveTriggerCallback(const std_msgs::Bool::ConstPtr& msg);  // "/drive_start" 토픽 콜백 함수
    void poseCallback(const turtlesim::Pose::ConstPtr& msg);    // "/turtle1/pose" 토픽 콜백 함수
    
    void setTargetPosition(double target_x, double target_y);             // 목표 지점 설정

    void driveStraight();                     // 직진
    // void driveDiagonal(double straight);                    // 사선 주행
    void changeDirection(double angle);                    // 방향 변경
    void driveCircle(double straight, double angle);      // 원형 주행

    ros::NodeHandle nh;                      // ROS 노드 핸들
    ros::Publisher pub;                      // 퍼블리셔
    ros::Subscriber drive_start_sub;            // "/drive_start" 토픽 구독 서브스크라이버
    ros::Subscriber pose_sub;                   // "/turtle1/pose" 토픽 구독 서브스크라이버
    ros::ServiceClient client_clear;                 // 서비스 클라이언트
    ros::ServiceClient client_kill;                 // 서비스 클라이언트
    ros::ServiceClient client_spawn;                 // 서비스 클라이언트

       
    public:
    TurtlesimDrive();                         // 생성자
    ~TurtlesimDrive();                        // 소멸자

};

#endif 