#include <iostream>
#include <chrono>
#include <cmath>
#include <deque>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>

// kontrol alanı (LiDAR'a göre, LiDAR alanın arka kenarının ortasında)
#define AREA_WIDTH_M   0.5   // y yönünde toplam genişlik
#define AREA_HEIGHT_M  0.5    // x yönünde (ileri) uzunluk

// hızlar
#define LINEAR_SPEED_MPS     0.8
#define ANGULAR_SPEED_RADPS  1.0

// dönüş yönleri (angular.z işareti: + sola, - sağa)
#define TURN_LEFT   1
#define TURN_RIGHT -1

// sıkışma tespiti için tutulacak hareket sayısı
#define MOTION_HISTORY_SIZE 5

enum class RobotState { GO_FORWARD, TURN, BREAK_LOOP };
// not: TURN_LEFT / TURN_RIGHT makro olduğu için enum değerleri farklı isimde
enum class Motion { FORWARD, LEFT, RIGHT };

class BumpAndGo : public rclcpp::Node
{
public:
    BumpAndGo();
private:
    // abonelik ve yayıncı
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub_;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr cmd_vel_pub_;

    // callback
    void scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg);

    // durum makinesi
    RobotState state_ = RobotState::GO_FORWARD;
    int turn_direction_ = TURN_LEFT;

    // son hareketlerin FIFO'su (sadece hareket değiştiğinde eklenir)
    std::deque<Motion> motion_history_;
    void add_motion_to_history(Motion motion);
    bool is_stuck_in_loop();
};
