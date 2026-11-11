#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#define WINDOW 500

class VisualizeLIDAR : public rclcpp::Node
{
public:
    VisualizeLIDAR();
private:
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscription_;
    void lidar_callback(const sensor_msgs::msg::LaserScan::SharedPtr);
};