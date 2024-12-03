#include "ojt_5/sensor_msgs_img.hpp"

SensorMsgsImg::SensorMsgsImg() {
}

SensorMsgsImg::~SensorMsgsImg() {
}

void SensorMsgsImg::subscribeImage(const std::string &topic, int queue_size) {
    sub_row_img = n.subscribe(topic, queue_size, &SensorMsgsImg::imagePubCallback, this);
}

void SensorMsgsImg::subscribeCompressedImage(const std::string &topic, int queue_size) {
    sub_comp_img = n.subscribe(topic, queue_size, &SensorMsgsImg::compSubCallback, this);
}

void SensorMsgsImg::advertiseCompressedImage(const std::string &topic, int queue_size) {
    pub_comp_img = n.advertise<sensor_msgs::CompressedImage>(topic, queue_size);
}

void SensorMsgsImg::imageSubCallback(const sensor_msgs::Image::ConstPtr &msg) {
    cv::Mat image = cv_bridge::toCvCopy(msg, "bgr8")->image;
    cv::imshow("Camera Image", image);
    cv::waitKey(1);
}

void SensorMsgsImg::compSubCallback(const sensor_msgs::CompressedImage::ConstPtr &msg) {
    std::vector<unsigned char> buffer(msg->data.begin(), msg->data.end());
    cv::Mat image = cv::imdecode(buffer, cv::IMREAD_COLOR);
    cv::imshow("Compressed Image", image);
    cv::waitKey(1);
}

void SensorMsgsImg::imagePubCallback(const sensor_msgs::Image::ConstPtr &msg) {
    cv::Mat image = cv_bridge::toCvCopy(msg, "bgr8")->image;

    std::vector<uchar> compressed_image;
    cv::imencode(".jpg", image, compressed_image);

    compressed_msg.header = msg->header;  
    compressed_msg.format = "jpeg";  
    compressed_msg.data = compressed_image;

    pub_comp_img.publish(compressed_msg);
}