#include <low_level_autonomy/vis_lidar.h>

VisualizeLIDAR::VisualizeLIDAR() : Node("vis_lidar")
{
    subscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>("/scan", 1, std::bind(&VisualizeLIDAR::lidar_callback, this, std::placeholders::_1));
}

void VisualizeLIDAR::lidar_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
{
    std::stringstream fmt;
    fmt << "Processing scan: " << msg->header.stamp.sec << "sec " << msg->header.stamp.nanosec << "nanosec";
    RCLCPP_INFO(this->get_logger(), "%s", fmt.str().c_str());
    char window_name[] = "Polar to Cartesian Laser Endpoints";
    cv::Mat image = cv::Mat::zeros(WINDOW, WINDOW, CV_8UC3);
    cv::circle(image, cv::Point(WINDOW / 2, WINDOW / 2), 2, cv::Scalar(255, 0, 0), cv::FILLED, cv::LINE_8);
    cv::arrowedLine(image, cv::Point(WINDOW / 2, WINDOW / 2), cv::Point(WINDOW / 2 + 10, WINDOW / 2), cv::Scalar(255, 0, 0), 1, 8, 0, 0.1);
    for (std::vector<float>::size_type i = 0; i < msg->ranges.size(); i++)
    {
        float angle = msg->angle_min + i * msg->angle_increment;
        float norm_range = (msg->ranges.at(i) / msg->range_max) * (WINDOW / 2);
        int x = (int)(WINDOW / 2 + norm_range * cos(angle));
        int y = (int)(WINDOW / 2 - norm_range * sin(angle));
        cv::circle(image, cv::Point(x, y), 1, cv::Scalar(0, 0, 255), cv::FILLED, cv::LINE_8);
    }
    cv::imshow(window_name, image);
    cv::waitKey(1);
}

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<VisualizeLIDAR>());
    rclcpp::shutdown();
    return 0;
}