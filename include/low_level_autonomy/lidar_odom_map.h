#include <iostream>
#include <chrono>
#include <cmath>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#define WINDOW 750
#define MT_TO_PIXEL 30

#define DEG_TO_RAD 0.01745329

class LIDAROdomMap : public rclcpp::Node
{
public:
    LIDAROdomMap();
private:
    // Abonelikler
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;

    void lidar_map_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg);
    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg);
    void update_map();

    double robot_x_m = 0.0;
    double robot_y_m = 0.0;
    double robot_yaw_rad = 0.0;

    cv::Mat map_;
};