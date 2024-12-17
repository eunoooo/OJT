#include "ojt_6/tf_sensor.h"

TFSensor::TFSensor() : tfListener(tfBuffer){

    sub_scan = n.subscribe("/scan", 10, &TFSensor::scanCallback, this);
   // sub_points = n.subscribe("/camera/depth/points", 10, pointsCallback);

    pub_transform_scan = n.advertise<sensor_msgs::PointCloud>("/ojt/transform/scan", 10);
    //pub_transform_scan = n.advertise<geometry_msgs::PointStamped>("/ojt/transform/scan", 10);
     printf("hi~2222/n");
}

TFSensor::~TFSensor(){
}


void TFSensor::scanCallback(const sensor_msgs::LaserScan::ConstPtr& msg){
  
    transform_stamped = tfBuffer.lookupTransform("base_link", "laser", ros::Time(0));

    // 극좌표계 -> 데카르트 좌표계
    std::vector<tf::Vector3> cartesian_points = convertCoordinate(msg);

    // 변환된 좌표계 데이터를 base_link 기준으로 TF 변환
    tf::Vector3 transformed_point;
    //pointCloud 변환할 때 사용할 변수
    geometry_msgs::Point32 p;
    sensor_msgs::PointCloud cloud_msg;

    cloud_msg.header.stamp = msg->header.stamp;  // 시간 정보
    cloud_msg.header.frame_id = "base_link";    // 변환된 프레임

    for (tf::Vector3 point : cartesian_points){
        transformed_point = TF(transform_stamped, point);  // TF 적용

        p.x = transformed_point.x();
        p.y = transformed_point.y();
        p.z = transformed_point.z();
        cloud_msg.points.push_back(p);
    }


    pub_transform_scan.publish(cloud_msg);
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

tf::Vector3 TFSensor::TF(const geometry_msgs::TransformStamped& transform_stamped, tf::Vector3 original_v){
    // 1. TransformStamped에서 쿼터니언 가져오기
    geometry_msgs::Quaternion geo_quat = transform_stamped.transform.rotation;

    // 2. 쿼터니언을 tf::Quaternion으로 변환
    tf::Quaternion tf_quat(geo_quat.x, geo_quat.y, geo_quat.z, geo_quat.w);

    // 3. 회전 행렬 (tf::Matrix3x3) 생성
    tf::Matrix3x3 rotation_matrix(tf_quat);

    // 4. 회전 축과 회전 각도 가져오기
    double roll, pitch, yaw;
    rotation_matrix.getRPY(roll, pitch, yaw); // RPY (Roll, Pitch, Yaw) 값을 가져옴

    // 5. 벡터 회전 변환
    tf::Vector3 rotated_v = rotation_matrix * original_v;

    // 6. 병진 변환(좌표계나 점을 특정 방향으로 일정한 거리만큼 평행 이동.) 적용
    geometry_msgs::Vector3 translation = transform_stamped.transform.translation;
    tf::Vector3 translated_v = rotated_v + tf::Vector3(translation.x, translation.y, translation.z);

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



/*
void TFSensor::pointsCallback(const sensor_msgs::PointCloud2::ConstPtr& msg){

}
*/


