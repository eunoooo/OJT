cmake_minimum_required(VERSION 3.0.2)
project(ojt_5)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  sensor_msgs
  image_transport
  cv_bridge
)

find_package(OpenCV REQUIRED)

catkin_package(
  CATKIN_DEPENDS roscpp sensor_msgs image_transport cv_bridge
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)

add_executable(image_sub_node src/image_sub_node.cpp)
add_executable(comp_sub_node src/comp_sub_node.cpp)
add_executable(comp_pub_node src/comp_pub_node.cpp)

target_link_libraries(image_sub_node ${catkin_LIBRARIES})
target_link_libraries(comp_sub_node ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(comp_pub_node ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})

