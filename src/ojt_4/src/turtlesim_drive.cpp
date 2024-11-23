#include "ros/ros.h"
#include "std_msgs/Bool.h"
#include "geometry_msgs/Twist.h"
#define PI 3.141592

class TurtlesimDrive
{
public:
    TurtlesimDrive();
    void driveTurtlesim();                    // 전체 주행 로직 함수
    void driveTriggerCallback(const std_msgs::Bool::ConstPtr& msg);  // 콜백 함수

    protected:
    void driveStraight();                     // 직진하는 함수
    void changeDirection(double angle);       // 방향 변경 함수
    void driveCircle(double angle);           // 원형으로 주행하는 함수

    private:
    ros::NodeHandle nh_;                      // ROS 노드 핸들
    ros::Publisher pub_;                      // 퍼블리셔
    ros::Subscriber sub_;                     // 서브스크라이버
};

TurtlesimDrive::TurtlesimDrive()
{
    // 퍼블리셔 초기화 ("/turtle1/cmd_vel" 토픽으로 퍼블리시)
    pub_ = nh_.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);

    // 서브스크라이버 초기화 ("/drive_start" 토픽 구독)
    sub_ = nh_.subscribe("/drive_start", 10, &TurtlesimDrive::driveTriggerCallback, this);
}

void TurtlesimDrive::driveStraight()
{
    geometry_msgs::Twist twist;
    twist.linear.x = 2.0;  // 직진 속도 설정, 단위는 1m/s ?
    ros::Duration(2.0).sleep();
    pub_.publish(twist);   // 메시지 퍼블리시
}

void TurtlesimDrive::changeDirection(double angle)
{
    geometry_msgs::Twist twist;
    twist.angular.z = angle;  // 회전 각도 설정
    twist.linear.x = 0.0;     // 직진 속도 0으로 설정
    ros::Duration(1.0).sleep();  // 1초간 회전함
    pub_.publish(twist);      // 메시지 퍼블리시
}

void TurtlesimDrive::driveCircle(double angle)
{
    geometry_msgs::Twist twist;
    twist.linear.x = 2.0;     // 직진 속도 설정
    twist.angular.z = angle;  // 회전 각도 설정
    pub_.publish(twist);      // 메시지 퍼블리시
}

void TurtlesimDrive::driveTurtlesim()
{
    driveStraight();        // 직진
    ros::Duration(1.0).sleep();  //회전하기 전에 1초 정지
    changeDirection(90*(PI/180));  // 방향 변경

    driveStraight();        // 직진
    ros::Duration(1.0).sleep();  //회전하기 전에 1초 정지
    changeDirection(135*(PI/180));  // 방향 변경

     driveStraight();        // 직진
    ros::Duration(1.0).sleep();  //회전하기 전에 1초 정지
    changeDirection(-(90*(PI/180)));  // 방향 변경

    driveStraight();        // 직진
    ros::Duration(1.0).sleep();  //회전하기 전에 1초 정지
    changeDirection(135*(PI/180));  // 방향 변경

     driveStraight();        // 직진
    ros::Duration(1.0).sleep();  //회전하기 전에 1초 정지
    changeDirection(90*(PI/180));  // 방향 변경

    driveStraight();        // 직진
    // driveStraight();
    // driveCircle(2.0);      // 원형 주행
}

void TurtlesimDrive::driveTriggerCallback(const std_msgs::Bool::ConstPtr& msg)
{
if (msg->data)  // "drive_start" 메시지가 true인 경우에만 주행 시작
    {
        driveTurtlesim();
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "turtlesim_drive");
    TurtlesimDrive drive;  // TurtlesimDrive 클래스 인스턴스 생성
    ros::spin();           // 콜백 처리를 위한 ROS 메인 루프

    return 0;
}