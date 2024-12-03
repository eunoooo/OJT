#include "ojt_4/turtlesim_drive_header.h"

TurtlesimDrive::TurtlesimDrive() {
    pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);

    pose_sub = nh.subscribe("/turtle1/pose", 10, &TurtlesimDrive::poseCallback, this);

    drive_start_sub = nh.subscribe("/drive_start", 10, &TurtlesimDrive::driveTriggerCallback, this);

    client_clear = nh.serviceClient<std_srvs::Empty>("/clear");
    client_kill = nh.serviceClient<turtlesim::Kill>("/kill");
    client_spawn = nh.serviceClient<turtlesim::Spawn>("/spawn");
}

TurtlesimDrive::~TurtlesimDrive() { 
}

void TurtlesimDrive::poseCallback(const turtlesim::Pose::ConstPtr& msg) {
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
    twist.linear.y = 0.0;          
    twist.linear.z = 0.0;

    twist.angular.x = 0.0;
    twist.angular.y = 0.0;
    twist.angular.z = 0.0;

    ros::Rate loop_rate(10);
    double dist = sqrt(pow(target_pose_x - current_pose_x, 2) + pow(target_pose_y - current_pose_y, 2));

    while (dist > 0.2) {
        dist = sqrt(pow(target_pose_x - current_pose_x, 2) + pow(target_pose_y - current_pose_y, 2));  
        pub.publish(twist);
        ros::spinOnce(); 
        loop_rate.sleep();
    }
    
    twist.linear.x = 0.0; 
    pub.publish(twist); 
}

void TurtlesimDrive::changeDirection(double angle) {
    twist.linear.x = 0.0; 
    twist.linear.y = 0.0;        
    twist.linear.z = 0.0;

    twist.angular.x = 0.0;
    twist.angular.y = 0.0;
    twist.angular.z = angle; 

    pub.publish(twist); 
    ros::Duration(1.0).sleep();
    ROS_INFO("changeDirection call : %lf", angle);
}

void TurtlesimDrive::driveCircle(double straight, double angle) {
    ros::Time start_time = ros::Time::now();  
    
    double du = (M_PI) / std::fabs(angle);  
    
    twist.linear.x = straight;     
    twist.linear.y = 0.0;          
    twist.linear.z = 0.0;

    twist.angular.x = 0.0;
    twist.angular.y = 0.0;
    twist.angular.z = angle;     

    pub.publish(twist);

    ros::Rate loop_rate(10);
    double dist = sqrt(pow(target_pose_x - current_pose_x, 2) + pow(target_pose_y - current_pose_y, 2));

    while (dist > 0.2) {
        pub.publish(twist);
        ros::spinOnce(); 
        loop_rate.sleep();
        dist = sqrt(pow(target_pose_x - current_pose_x, 2) + pow(target_pose_y - current_pose_y, 2));  
    }

    twist.linear.x = 0.0;
    twist.angular.z = 0.0; 
    pub.publish(twist); 
}

void TurtlesimDrive::driveTurtlesim() {
    std_srvs::Empty srv_clear;    
    turtlesim::Kill srv_kill;    
    srv_kill.request.name = "turtle1";

    turtlesim::Spawn srv_spawn; 
    srv_spawn.request.x = 5.5;
    srv_spawn.request.y = 5.5;
    srv_spawn.request.name = "turtle1"; 


    setTargetPosition(9.5, 5.5);  
    driveStraight();       
    ros::Duration(1.0).sleep();  
    changeDirection(90*(M_PI/180));  

    setTargetPosition(9.5, 9.5); 
    driveStraight();     
    ros::Duration(1.0).sleep();  
    changeDirection((131.2)*(M_PI/180));  

    setTargetPosition(5.5, 6.0);  
    driveStraight();       
    ros::Duration(1.0).sleep(); 
    changeDirection(-((82.4)*(M_PI/180)));    


    setTargetPosition(1.5, 9.5); 
    ros::Duration(1.0).sleep();
    driveStraight();       
    ros::Duration(1.0).sleep();  
    changeDirection((131.2)*(M_PI/180));  

    setTargetPosition(1.5, 5.5); 
    ros::Duration(1.0).sleep();
    driveStraight();       
    ros::Duration(1.0).sleep(); 
    changeDirection(90*(M_PI/180));  

    setTargetPosition(5.5, 5.5);  
    driveStraight();       
    ros::Duration(1.0).sleep();


    client_kill.call(srv_kill);

    client_spawn.call(srv_spawn);

    client_clear.call(srv_clear);
    

    setTargetPosition(5.5, 9.5);  
    driveCircle(2.0, 1.0);    
    setTargetPosition(5.5, 5.5); 
    driveCircle(2.0, 1.0);
    ros::Duration(1.0).sleep();

    setTargetPosition(5.5, 1.5);  
    driveCircle(2.0, -1.0);
    ros::Duration(1.0).sleep();

    setTargetPosition(5.5, 3.5);  
    driveCircle(1.0, -1.0);
    ros::Duration(1.0).sleep();

    setTargetPosition(5.5, 5.5);  
    driveCircle(1.0, 1.0);
}

void TurtlesimDrive::driveTriggerCallback(const std_msgs::Bool::ConstPtr& msg) {
    if (msg->data) {   
        driveTurtlesim();    
    }
}