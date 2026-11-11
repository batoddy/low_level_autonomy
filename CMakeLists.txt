cmake_minimum_required(VERSION 3.8)
project(low_level_autonomy)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(include)

add_executable(vis_lidar_node src/vis_lidar.cpp)
ament_target_dependencies(vis_lidar_node rclcpp sensor_msgs OpenCV)
install(TARGETS
  vis_lidar_node
  DESTINATION lib/${PROJECT_NAME})

add_executable(lidar_odom_map_node src/lidar_odom_map.cpp)
ament_target_dependencies(lidar_odom_map_node rclcpp sensor_msgs nav_msgs OpenCV)
install(TARGETS
  lidar_odom_map_node
  DESTINATION lib/${PROJECT_NAME})

add_executable(bump_go_node src/bump_go.cpp)
ament_target_dependencies(bump_go_node rclcpp sensor_msgs geometry_msgs)
install(TARGETS
  bump_go_node
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}/
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
