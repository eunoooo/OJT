#include "ojt_4/turtlesim_drive_header.h"


int main(int argc, char **argv) {       // ROS 노드 실행의 진입점
    ros::init(argc, argv, "turtlesim_drive");   // ROS 노드를 초기화, 노드 이름을 설정
    TurtlesimDrive drive;   // TurtlesimDrive 클래스의 객체 **drive**를 생성
    ros::spin();       // ROS 콜백 함수들을 처리하는 무한 루프. 정확히는 콜백 함수를 실행시켜 큐를 털어준다
    // ros::spinOnce() 를 쓰면 프로그램이 바로 종료되어 버림

    return 0;
}