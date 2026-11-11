#include <low_level_autonomy/lidar_odom_map.h>

LIDAROdomMap::LIDAROdomMap() : Node("lidar_odom_map")
{
    scan_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>("/scan", 1, std::bind(&LIDAROdomMap::lidar_map_callback, this, std::placeholders::_1));
    odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>("/odom", 10, std::bind(&LIDAROdomMap::odom_callback, this, std::placeholders::_1));
    
    
    map_ = cv::Mat::zeros(WINDOW, WINDOW, CV_8UC3);

}

void LIDAROdomMap::odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg)
{
    robot_x_m   = msg->pose.pose.position.x;
    robot_y_m   = msg->pose.pose.position.y;
    double q_z = msg->pose.pose.orientation.z;
    double q_w = msg->pose.pose .orientation.w;
    robot_yaw_rad = atan2(2.0 * q_w * q_z, 1.0 - 2.0 * q_z * q_z);

    int robot_x_pixel = WINDOW/2 + (robot_x_m * MT_TO_PIXEL); 
    int robot_y_pixel = WINDOW/2 - (robot_y_m * MT_TO_PIXEL);  

    cv::circle(map_, cv::Point(robot_x_pixel, robot_y_pixel), 1, cv::Scalar(255, 0, 0), cv::FILLED, cv::LINE_8);

    update_map();
}

void LIDAROdomMap::lidar_map_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
{
    // robotun yaw'ı bir tarama boyunca değişmiyor, döngüden önce bir kez hesapla
    double cos_yaw = cos(robot_yaw_rad);
    double sin_yaw = sin(robot_yaw_rad);

    for(std::vector<float>::size_type i = 0; i < msg->ranges.size(); i++)
    {
        double angle_rad = msg->angle_min + i * msg->angle_increment;
        double range_m = msg->ranges.at(i);

        // geçersiz ölçüm varsa atlanır (inf/nan hatası için)
        if(std::isfinite(range_m) && range_m > msg->range_min && range_m < msg->range_max)
        {
            // base_scan - lidara göre
            double scan_x_m = range_m * cos(angle_rad);
            double scan_y_m = range_m * sin(angle_rad);

            // base_footprint - robota göre
            double base_x_m = scan_x_m - 0.032;
            double base_y_m = scan_y_m;

            // nokta robotun yaw'ı kadar döndürülür 
            double rotated_x_m = cos_yaw * base_x_m - sin_yaw * base_y_m;
            double rotated_y_m = sin_yaw * base_x_m + cos_yaw * base_y_m;

            // odom çerçevesinde nokta (robotun konumu kadar kaydırılır)
            double odom_x_m = robot_x_m + rotated_x_m;
            double odom_y_m = robot_y_m + rotated_y_m;

            int x_pixel = WINDOW/2 + (odom_x_m * MT_TO_PIXEL);
            int y_pixel = WINDOW/2 - (odom_y_m * MT_TO_PIXEL);
            cv::circle(map_, cv::Point(x_pixel, y_pixel), 1, cv::Scalar(0, 0, 255), cv::FILLED, cv::LINE_8);
        }
    }
    update_map();
}

void LIDAROdomMap::update_map()
{
    const char window_name[] = "Map";

    cv::imshow(window_name, map_);
    cv::waitKey(1);
}

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<LIDAROdomMap>());
    rclcpp::shutdown();
    return 0;
}