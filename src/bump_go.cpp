#include <low_level_autonomy/bump_go.h>

BumpAndGo::BumpAndGo() : Node("bum_go")
{

}

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<BumpAndGo>());
    rclcpp::shutdown();
    return 0;
}