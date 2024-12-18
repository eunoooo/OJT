#include "ojt_6/tf_sensor.h"

TFSensor::TFSensor(){

    sub_scan = n.subscribe("/scan", 1, &TFSensor::scanCallback, this);
    sub_points = n.subscribe("/camera/depth/points", 1, &TFSensor::pointsCallback, this);

    pub_transform_scan = n.advertise<sensor_msgs::PointCloud>("/ojt/transform/scan", 10);
    pub_transform_depth = n.advertise<sensor_msgs::PointCloud>("/ojt/transform/depth", 10);

   // printf("hi~2222/n");
}

TFSensor::~TFSensor(){
}


void TFSensor::scanCallback(const sensor_msgs::LaserScan::ConstPtr& msg){
  
    // transform_stamped_laser = tfBuffer.lookupTransform("base_link", "laser_link", ros::Time(0));
    listener.lookupTransform("base_link", "laser_link", ros::Time(0), transform_stamped_laser);

    // 극좌표계 -> 데카르트 좌표계
    std::vector<tf::Vector3> cartesian_points = convertCoordinate(msg);

    // 변환된 좌표계 데이터를 base_link 기준으로 TF 변환
    tf::Vector3 transformed_point_laser;
    // pointCloud 변환할 때 사용할 변수
    geometry_msgs::Point32 p;
    sensor_msgs::PointCloud cloud_msg;

    cloud_msg.header.stamp = ros::Time::now();
    cloud_msg.header.frame_id = "base_link";


    for(tf::Vector3 point : cartesian_points){
        transformed_point_laser = TF(transform_stamped_laser, point);  // TF 적용

        p.x = transformed_point_laser.x();
        p.y = transformed_point_laser.y();
        p.z = transformed_point_laser.z();
        cloud_msg.points.push_back(p);
    }

    pub_transform_scan.publish(cloud_msg);
}

/*


 */
void TFSensor::pointsCallback(const sensor_msgs::PointCloud2::ConstPtr& msg){
    // TF 변환을 위한 TransformStamped 가져오기
    // transform_stamped_camera = tfBuffer.lookupTransform("base_link", "camera_link", ros::Time(0));

    listener.lookupTransform("base_link", "camera_link", ros::Time(0), transform_stamped_camera);
    
    // pointCloud 변환할 때 사용할 변수
    geometry_msgs::Point32 p;
    sensor_msgs::PointCloud camera_msg;

    camera_msg.header.stamp = ros::Time::now();
    camera_msg.header.frame_id = "base_link";  
    
    // todo pointcloud2 iterator xyz 접근 어케 함ㅜㅜ!!
    sensor_msgs::PointCloud2ConstIterator<float> iter_x(*msg, "x");
	sensor_msgs::PointCloud2ConstIterator<float> iter_y(*msg, "y");
	sensor_msgs::PointCloud2ConstIterator<float> iter_z(*msg, "z");
     
    tf::Vector3 transformed_point_camera;

    while(iter_x != iter_x.end()){
        tf::Vector3 point(*iter_x, *iter_y, *iter_z);
        tf::Vector3 transformed_point_camera = TF(transform_stamped_camera, point);

            p.x = transformed_point_camera.x();
            p.y = transformed_point_camera.y();
            p.z = transformed_point_camera.z();

            camera_msg.points.push_back(p); 

            ++iter_x;
            ++iter_y;
            ++iter_z;
    }
    

    // 변환된 PointCloud2 메시지 publish
    pub_transform_depth.publish(camera_msg);
}

std::vector<tf::Vector3> TFSensor::convertCoordinate(const sensor_msgs::LaserScan::ConstPtr& msg){
    std::vector<tf::Vector3> cartesian_points;  // 데카르트 좌표계로 변환된 점들을 저장

    for(int i = 0 ; i < msg->ranges.size() ; i++){
        angle = msg->angle_min + (msg->angle_increment * i);
        distance = msg->ranges[i];

        x = distance * cos(angle);
        y = distance * sin(angle);
        z = 0.0;  // Z는 0으로 설정 (2D 평면)
        
        // convertXYZ.push_back(make_tuple(x, y, z));
        cartesian_points.push_back(tf::Vector3(x, y, z));
    }

    return cartesian_points;  // 변환된 좌표 목록 반환
}

tf::Vector3 TFSensor::TF(const tf::StampedTransform& transform_stamped, tf::Vector3 original_v){
    // TransformStamped에서 쿼터니언 가져오기
    // geometry_msgs::Quaternion geo_quat = transform_stamped.transform.rotation;

    // 쿼터니언을 tf::Quaternion으로 변환
    tf::Quaternion tf_quat = transform_stamped.getRotation();

    // 회전 행렬 (tf::Matrix3x3) 생성
    tf::Matrix3x3 rotation_matrix(tf_quat);

    // 벡터 회전 변환
    tf::Vector3 rotated_v = rotation_matrix * original_v;

    // 회전 변환 + 직선 변환
    tf::Vector3 translated_v = rotated_v + tf::Vector3(transform_stamped.getOrigin().x(), transform_stamped.getOrigin().y(), transform_stamped.getOrigin().z());

    return translated_v;
}


// void TFSensor::TF(geometry_msgs::TransformStamped transform_stamped){
//     //todo transform_stamped의 회전 축과 값을 가져와 변수에 저장
//     float tf_angle = transform_stamped; 

//     tf::Matrix3x3 rotation_m_x(
//         1,           0,          0,
//         0,  cos(angle), -sin(angle),
//         0,  sin(angle),  cos(angle)
//     );
//     tf::Matrix3x3 rotation_m_y(
//         cos(angle),  0, sin(angle),
//         0,           1,          0,
//         -sin(angle), 0, cos(angle)
//     );
//     tf::Matrix3x3 rotation_m_z(
//         cos(angle), -sin(angle), 0,
//         sin(angle),  cos(angle), 0,
//         0,           0,          1
//     );

//     tf::Vector3 original_v;

// }







