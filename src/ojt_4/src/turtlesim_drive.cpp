#include "ojt_4/turtlesim_drive_header.h"

TurtlesimDrive::TurtlesimDrive() {
    // 퍼블리셔 초기화 ("/turtle1/cmd_vel" 토픽으로 퍼블리시)
    pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);

    // 서브스크라이버 초기화 ("/drive_start" 토픽 구독)
    drive_start_sub = nh.subscribe("/drive_start", 10, &TurtlesimDrive::driveTriggerCallback, this);

    // 서브스크라이버 초기화 ("/turtle1/pose" 토픽 구독)
    pose_sub = nh.subscribe("/turtle1/pose", 10, &TurtlesimDrive::poseCallback, this);

    // 서비스 클라이언트 초기화 ("/clear" 서비스와 연결)
    client = nh.serviceClient<std_srvs::Empty>("/clear");
}

TurtlesimDrive::~TurtlesimDrive() { 
}

void TurtlesimDrive::poseCallback(const turtlesim::Pose::ConstPtr& msg) {
   //현재 좌표 설정
   current_pose_x = msg->x;
   current_pose_y = msg->y;
   ROS_INFO("Current position: (%.2f, %.2f)", current_pose_x, current_pose_y);
}

void TurtlesimDrive::setTargetPosition(double x, double y) {
    target_pose_x = x;
    target_pose_y = y;
}

void TurtlesimDrive::driveStraight(double straight) {
    ros::Time start_time = ros::Time::now();  // 시작 시간 초기화
    
    double du = (M_PI) / std::fabs(angle);  // 직진 주행 시간 계산

    twist.linear.x = straight; 
    twist.linear.y = 0.0;          // twist의 다른 성분들에 대한 초기화
    twist.linear.z = 0.0;

    twist.angular.x = 0.0;
    twist.angular.y = 0.0;
    twist.angular.z = 0.0;

    while ((ros::Time::now() - start_time).toSec() < du) {
        pub.publish(twist);   
    }
    
    twist.linear.x = 0.0; 
    pub.publish(twist); 
}

void TurtlesimDrive::driveDiagonal(double diagonal) {

    twist.linear.x = straight; 
    twist.linear.y = 0.0;          // twist의 다른 성분들에 대한 초기화
    twist.linear.z = 0.0;

    twist.angular.x = 0.0;
    twist.angular.y = 0.0;
    twist.angular.z = 0.0;

    while (sqrt(pow(target_pose_x - current_pose_x, 2) + pow(target_pose_y - current_pose_y, 2)) > 0.1) {   // 0.1은 실수형 좌표 비교 시 허용 오차
        pub.publish(twist);
    }
    
    twist.linear.x = 0.0; 
    pub.publish(twist); 
}


void TurtlesimDrive::changeDirection(double angle) {
    twist.linear.x = 0.0; 
    twist.linear.y = 0.0;          // twist의 다른 성분들에 대한 초기화
    twist.linear.z = 0.0;

    twist.angular.x = 0.0;
    twist.angular.y = 0.0;
    twist.angular.z = angle;  // 회전 각도 설정

    pub.publish(twist); 
    ros::Duration(1.0).sleep();
}

void TurtlesimDrive::driveCircle(double straight, double angle) {
    ros::Time start_time = ros::Time::now();  // 시작 시간 초기화
    
    double du = (M_PI) / std::fabs(angle);  // 원형 주행 시간 계산
    
    twist.linear.x = straight;     // 직진 속도 설정
    twist.linear.y = 0.0;          // twist의 다른 성분들에 대한 초기화
    twist.linear.z = 0.0;

    twist.angular.x = 0.0;
    twist.angular.y = 0.0;
    twist.angular.z = angle;       // 회전 각도 설정

    while((ros::Time::now() - start_time).toSec() < du) {
        pub.publish(twist);   
    }
}

void TurtlesimDrive::driveTurtlesim() {
    std_srvs::Empty srv;    // clear 서비스 요청을 위한 빈 서비스 메세지 생성

    driveStraight(4.0);        // 직진
    ros::Duration(1.0).sleep();  //회전하기 전에 1초 정지
    changeDirection(90*(M_PI/180));  // 방향 변경

    driveStraight(4.0);        // 직진
    ros::Duration(1.0).sleep();  //회전하기 전에 1초 정지
    changeDirection(135*(M_PI/180));  // 방향 변경

    setTargetPosition(5.5, 6.0);
    driveStraight(4.0);        // 대각선 방향으로 직진
    ros::Duration(1.0).sleep();  //회전하기 전에 1초 정지
    changeDirection(-(90*(M_PI/180)));  // 방향 변경

    driveStraight(4.0);        // 대각선 방향으로 직진
    ros::Duration(1.0).sleep();  //회전하기 전에 1초 정지
    changeDirection(135*(M_PI/180));  // 방향 변경

    driveStraight(4.0);        // 직진
    ros::Duration(1.0).sleep();  //회전하기 전에 1초 정지
    changeDirection(90*(M_PI/180));  // 방향 변경

    driveStraight(4.0);        // 직진
    /*
    // 터틀심 초기화
    client_.call(srv);

    
    driveCircle(2.0, 1.0);      // 원형 주행
    driveCircle(2.0, 1.0);
    ros::Duration(1.0).sleep();

    driveCircle(2.0, -1.0);
    ros::Duration(1.0).sleep();

    driveCircle(1.0, -1.0);
    ros::Duration(1.0).sleep();

    driveCircle(1.0, 1.0);
    */
}

void TurtlesimDrive::driveTriggerCallback(const std_msgs::Bool::ConstPtr& msg) {
    if (msg->data) {    // "drive_start" 메시지가 true인 경우에만 주행 시작
        driveTurtlesim();    
    }
}
