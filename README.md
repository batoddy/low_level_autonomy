# low_level_autonomy

Package contains a bag file with LiDAR and odometry messagges from turtlebot3 manually controlled in turtlebot3_house world. 

There are three nodes which are for LiDAR visualization, odom LiDAR mapping and Bump&Go behaviour.

## Step by Step

```
$ cd ~/ros2_ws/src
$ ros2 pkg create --build-type ament_cmake low_level_autonomy --dependencies rclcpp sensor_msgs nav_msgs geometry_msgs OpenCV
$ cd low_level_autonomy
$ touch src/vis_lidar.cpp
$ touch include/low_level_autonomy/vis_lidar.h

$ touch src/lidar_odom_map.cpp
$ touch include/low_level_autonomy/lidar_odom_map.h

$ mkdir launch
$ touch launch/house.launch.py
$ touch src/bump_go.cpp
$ touch include/low_level_autonomy/bump_go.h
```

Edit launch file, source files and then edit CMakeLists.txt (shown below)
```
...
include_directories(include)

add_executable(vis_lidar_node src/vis_lidar.cpp)
ament_target_dependencies(vis_lidar_node rclcpp sensor_msgs OpenCV)
install(TARGETS vis_lidar_node DESTINATION lib/${PROJECT_NAME})

add_executable(lidar_odom_map_node src/lidar_odom_map.cpp)
ament_target_dependencies(lidar_odom_map_node rclcpp sensor_msgs nav_msgs OpenCV)
install(TARGETS lidar_odom_map_nodeDESTINATION lib/${PROJECT_NAME})

add_executable(bump_go_node src/bump_go.cpp)
ament_target_dependencies(bump_go_node rclcpp sensor_msgs geometry_msgs)
install(TARGETS
  bump_go_node
  DESTINATION lib/${PROJECT_NAME})
...
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}/
)

```

Compile
```
$ cd ~/ros2_ws
$ colcon build --packages-select low_level_autonomy
$ source ~/.bashrc
```

## Install from Gitlab Repo

```
$ cd ~/ros2_ws/src
$ git clone https://gitlab.com/blm6191_2627g/blm6191/low_level_autonomy.git
$ cd ~/ros2_ws
$ colcon build --packages-select low_level_autonomy
$ source ~/.bashrc
```

## Run

Test LiDAR visualization

```
$ rviz2 -d ~/ros2_ws/src/low_level_autonomy/rviz/vis_lidar.rviz

CTRL+ALT+T
$ ros2 run low_level_autonomy vis_lidar_node

CTRL+ALT+T
$ ros2 bag play ~/ros2_ws/src/low_level_autonomy/bag/hw1

```

Test odom LiDAR mapping
```
$ ros2 run low_level_autonomy lidar_odom_map_node

CTRL+ALT+T
$ ros2 bag play ~/ros2_ws/src/low_level_autonomy/bag/hw1

```

Test bump and go
```
$ sudo apt install ros-jazzy-turtlebot3*
$ echo "export TURTLEBOT3_MODEL=burger" >> ~/.bashrc
$ source ~/.bashrc
$ ros2 launch low_level_autonomy house.launch.py

CTRL+ALT+T
$ ros2 run low_level_autonomy bump_go_node

```