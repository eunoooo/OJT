#ifndef OJT_4_TURTLESIM_DRIVE_HEADER     // 헤더 파일의 다중 포함을 방지하기 위한 include guard   
#define OJT_4_TURTLESIM_DRIVE_HEADER     

#include "ros/ros.h"
#include "std_msgs/Bool.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Pose.h"     
#include "turtlesim/Kill.h"
#include "turtlesim/Spawn.h"
#include "std_srvs/Empty.h"
#include <iostream>
#include <math.h>

class TurtlesimDrive {
    private:        // 클래스 내부에서만 접근 가능한 변수와 함수
    geometry_msgs::Twist twist;
    double current_pose_x = 0.0;        // 현재 거북이의 x,y 좌표와 회전각도를 저장
    double current_pose_y = 0.0;
    double current_theta = 0.0;
    double target_pose_x = 0.0;        // 목표 위치의 x, y 좌표를 저장
    double target_pose_y = 0.0;
 
    void driveTurtlesim();       // Turtlesim을 제어하는 메인 함수.          

    void driveTriggerCallback(const std_msgs::Bool::ConstPtr& msg);  // Boolean 메시지를 수신하여 Turtlesim을 시작하거나 멈추는 콜백 함수.
    void poseCallback(const turtlesim::Pose::ConstPtr& msg);   // Turtlesim의 현재 위치 정보를 수신하는 콜백 함수.
    void setTargetPosition(double target_x, double target_y);    // 목표 위치를 설정하는 함수.    

    void driveStraight();                    // 직선 주행을 구현하는 함수.
    void changeDirection(double angle);       // 주어진 각도로 방향을 변경하는 함수. 
    void driveCircle(double straight, double angle);     // Turtlesim을 원형 경로로 이동시키는 함수.
    
    ros::NodeHandle nh;        // ROS 노드와 통신하기 위한 핸들.              
    ros::Publisher pub;       // Turtlesim의 속도 명령을 퍼블리시.               
    ros::Subscriber drive_start_sub;    // Boolean 메시지를 구독하여 제어를 트리거.     
    ros::Subscriber pose_sub;        // 위치 정보를 구독하여 현재 상태를 업데이트.    
    ros::ServiceClient client_clear;        // 시뮬레이터 화면을 초기화.  
    ros::ServiceClient client_kill;      // 특정 Turtlesim 개체를 제거.    
    ros::ServiceClient client_spawn;       // 새 Turtlesim 개체를 생성. 

    public:         // 클래스 외부에서도 접근 가능한 인터페이스(생성자, 소멸자)
    TurtlesimDrive();        // 생성자 : 객체가 생성될 때 호출되어 초기화를 담당하는 함수
    ~TurtlesimDrive();      // 객체가 소멸될 때 호출되어 자원을 정리하거나 해제하는 함수
};

#endif 