#include "ojt_4/turtlesim_drive_header.h"

TurtlesimDrive::TurtlesimDrive() {
    // 퍼블리셔 초기화 ("/turtle1/cmd_vel" 토픽으로 퍼블리시)
    pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);

    // 서브스크라이버 초기화 ("/turtle1/pose" 토픽 구독)
    pose_sub = nh.subscribe("/turtle1/pose", 10, &TurtlesimDrive::poseCallback, this);

    // 서브스크라이버 초기화 ("/drive_start" 토픽 구독)
    drive_start_sub = nh.subscribe("/drive_start", 10, &TurtlesimDrive::driveTriggerCallback, this);

    // 서비스 클라이언트 초기화 ("/clear" "/kill" "/spawn" 서비스와 연결)
    client_clear = nh.serviceClient<std_srvs::Empty>("/clear");
    client_kill = nh.serviceClient<turtlesim::Kill>("/kill");
    client_spawn = nh.serviceClient<turtlesim::Spawn>("/spawn");
}

TurtlesimDrive::~TurtlesimDrive() { 
}

void TurtlesimDrive::poseCallback(const turtlesim::Pose::ConstPtr& msg) {
   // 현재 좌표 설정
    current_pose_x = msg->x;
    current_pose_y = msg->y;
    current_theta = msg->theta;
    ROS_INFO("call poseCallback : (%.2f, %.2f)", current_pose_x, current_pose_y);
}

void TurtlesimDrive::setTargetPosition(double target_x, double target_y) {
    target_pose_x = target_x;
    target_pose_y = target_y;
    ROS_INFO("call setTargetPosition : (%.2f, %.2f)", target_pose_x, target_pose_y);
}




void TurtlesimDrive::driveStraight() {

    twist.linear.x = 1.0; 
    twist.linear.y = 0.0;          // twist의 다른 성분들에 대한 초기화
    twist.linear.z = 0.0;

    twist.angular.x = 0.0;
    twist.angular.y = 0.0;
    twist.angular.z = 0.0;

    // 0.1은 실수형 좌표 비교 시 허용 오차
    // Rate 쓰는 이유 : rate 없이 while이 계속 돌면 너무 많은 CPU 자원을 쓰게 됨
    // 유클리드 거리 공식으로 점과 점 사이의 거리를 구한다.
    ros::Rate loop_rate(10);
    double dist = sqrt(pow(target_pose_x - current_pose_x, 2) + pow(target_pose_y - current_pose_y, 2));

    //todo while 안에 while을 쓰는 건 일반적이지 않음. 수정요함
    while (dist > 0.2) {
        dist = sqrt(pow(target_pose_x - current_pose_x, 2) + pow(target_pose_y - current_pose_y, 2));  
        ROS_INFO("current x : %lf current y : %lf \n", current_pose_x, current_pose_y); 
        ROS_INFO("target - current : %lf \n", dist); 
        pub.publish(twist);
        ros::spinOnce(); 
        loop_rate.sleep();
        // ROS_INFO("Current position: (%.2f, %.2f)", current_pose_x, current_pose_y);
    }
    
    twist.linear.x = 0.0; 
    pub.publish(twist); 
}

/*
void TurtlesimDrive::driveStraight(double straight) {

    twist.linear.x = straight; 
    twist.linear.y = 0.0;          // twist의 다른 성분들에 대한 초기화
    twist.linear.z = 0.0;

    twist.angular.x = 0.0;
    twist.angular.y = 0.0;
    twist.angular.z = 0.0;

    ros::Duration(2.0).sleep();
    pub.publish(twist);   // 메시지 퍼블리시
}
*/


void TurtlesimDrive::changeDirection(double angle) {
    twist.linear.x = 0.0; 
    twist.linear.y = 0.0;          // twist의 다른 성분들에 대한 초기화
    twist.linear.z = 0.0;

    twist.angular.x = 0.0;
    twist.angular.y = 0.0;
    twist.angular.z = angle;  // 회전 각도 설정

    pub.publish(twist); 
    ros::Duration(1.0).sleep();
    ROS_INFO("changeDirection call : %lf", angle);
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

    pub.publish(twist);

    ros::Rate loop_rate(10);
    double dist = sqrt(pow(target_pose_x - current_pose_x, 2) + pow(target_pose_y - current_pose_y, 2));

    while (dist > 0.2) {
        ROS_INFO("current x : %lf current y : %lf \n", current_pose_x, current_pose_y); 
        ROS_INFO("target - current : %lf \n", dist); 
        pub.publish(twist);
        ros::spinOnce(); 
        loop_rate.sleep();
        dist = sqrt(pow(target_pose_x - current_pose_x, 2) + pow(target_pose_y - current_pose_y, 2));  
        // ROS_INFO("Current position: (%.2f, %.2f)", current_pose_x, current_pose_y);
    }

    twist.linear.x = 0.0;
    twist.angular.z = 0.0; 
    pub.publish(twist); 
}




/*
// todo 원의 저 지점에서 stop
void TurtlesimDrive::driveCircle(double straight, double angle) {
   ros::Time start_time = ros::Time::now();
    
    double du = (M_PI) / std::fabs(angle);  // 원형 주행 시간 계산
    
    twist.linear.x = straight;     // 직진 속도 설정
    twist.linear.y = 0.0;          // twist의 다른 성분들에 대한 초기화
    twist.linear.z = 0.0;

    twist.angular.x = 0.0;
    twist.angular.y = 0.0;
    twist.angular.z = angle;       // 회전 각도 설정


    ros::Time current_time = ros::Time::now();  // 시작 시간 초기화
    while((current_time - start_time).toSec() < du) {
        pub.publish(twist);   
    }
    twist.linear.x = 0;
    twist.angular.z = 0;
}
 

*/





void TurtlesimDrive::driveTurtlesim() {
    std_srvs::Empty srv_clear;    // clear 서비스 요청을 위한 빈 서비스 메세지 생성
    turtlesim::Kill srv_kill;    // kill 서비스 요청용
    srv_kill.request.name = "turtle1";

    turtlesim::Spawn srv_spawn; // spawn 서비스 요청용
    srv_spawn.request.x = 5.5;
    srv_spawn.request.y = 5.5;
    srv_spawn.request.name = "turtle1"; // 터틀 이름


    setTargetPosition(9.5, 5.5);  // 주행 시 도달할 위치 설정
    driveStraight();        // 직진
    ros::Duration(1.0).sleep();  //회전하기 전에 1초 정지
    changeDirection(90*(M_PI/180));  // 방향 변경

    setTargetPosition(9.5, 9.5);  // 주행 시 도달할 위치 설정
    driveStraight();        // 직진
    ros::Duration(1.0).sleep();  //회전하기 전에 1초 정지
    changeDirection((131.2)*(M_PI/180));  // 방향 변경 
    // todo 원하는 목적지까지 닿을 수 있게 회전 방향 수정하기

    setTargetPosition(5.5, 6.0);  // 주행 시 도달할 위치 설정
    driveStraight();        // 직진
    ros::Duration(1.0).sleep();  //회전하기 전에 1초 정지
    changeDirection(-((82.4)*(M_PI/180)));  // 방향 변경        // todo 수정


    setTargetPosition(1.5, 9.5);  // 주행 시 도달할 위치 설정
    ros::Duration(1.0).sleep();
    driveStraight();        // 직진
    ros::Duration(1.0).sleep();  //회전하기 전에 1초 정지
    changeDirection((131.2)*(M_PI/180));  // 방향 변경

    setTargetPosition(1.5, 5.5);  // 주행 시 도달할 위치 설정
    ros::Duration(1.0).sleep();
    driveStraight();        // 직진
    ros::Duration(1.0).sleep();  //회전하기 전에 1초 정지
    changeDirection(90*(M_PI/180));  // 방향 변경

    setTargetPosition(5.5, 5.5);  // 주행 시 도달할 위치 설정
    driveStraight();        // 직진
    ros::Duration(1.0).sleep();

    //직선주행

    
    // 터틀심 kill
    client_kill.call(srv_kill);
    // 새로 스폰
    client_spawn.call(srv_spawn);

    
    // 터틀심 초기화
    client_clear.call(srv_clear);
    
    setTargetPosition(5.5, 9.5);  // 주행 시 도달할 위치 설정
    driveCircle(2.0, 1.0);      // 원형 주행
    setTargetPosition(5.5, 5.5);  // 주행 시 도달할 위치 설정
    driveCircle(2.0, 1.0);
    ros::Duration(1.0).sleep();

    setTargetPosition(5.5, 1.5);  // 주행 시 도달할 위치 설정
    driveCircle(2.0, -1.0);
    ros::Duration(1.0).sleep();

    setTargetPosition(5.5, 3.5);  // 주행 시 도달할 위치 설정
    driveCircle(1.0, -1.0);
    ros::Duration(1.0).sleep();

    setTargetPosition(5.5, 5.5);  // 주행 시 도달할 위치 설정
    driveCircle(1.0, 1.0);
    
}

// todo 이 부분은 driveTurtlesim 함수를 다른 방법으로 호출하게 바꾸기

void TurtlesimDrive::driveTriggerCallback(const std_msgs::Bool::ConstPtr& msg) {
    if (msg->data) {    // "drive_start" 메시지가 true인 경우에만 주행 시작
        driveTurtlesim();    
    }
}

