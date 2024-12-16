#include "ojt_4/turtlesim_drive_header.h"


int main(int argc, char **argv) {
    ros::init(argc, argv, "turtlesim_drive");
    TurtlesimDrive drive;  // TurtlesimDrive 클래스 인스턴스 생성
   // ros::spin();           // 콜백 처리를 위한 ROS 메인 루프
    while (ros::ok()) {
        ros::spinOnce();  // 콜백 처리
    }
    return 0;
}