#include <iostream>
#include <chrono>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>

class BumpAndGo : public rclcpp::Node
{
public:
    BumpAndGo();
private:

};