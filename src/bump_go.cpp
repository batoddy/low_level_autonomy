#include <low_level_autonomy/bump_go.h>

BumpAndGo::BumpAndGo() : Node("bump_go")
{
    scan_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>("/scan", 1, std::bind(&BumpAndGo::scan_callback, this, std::placeholders::_1));
    cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::TwistStamped>("/cmd_vel", 10);
}

void BumpAndGo::scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
{
    int left_point_count = 0;
    int right_point_count = 0;

    for(std::vector<float>::size_type i = 0; i < msg->ranges.size(); i++)
    {
        double angle_rad = msg->angle_min + i * msg->angle_increment;
        double range_m = msg->ranges.at(i);

        double scan_x_m = range_m * cos(angle_rad);
        double scan_y_m = range_m * sin(angle_rad);

        bool in_area = scan_x_m > 0.0 && scan_x_m < AREA_HEIGHT_M && fabs(scan_y_m) < AREA_WIDTH_M / 2.0;
        if(!in_area)
            continue;

        if(scan_y_m > 0.0)
            left_point_count++;
        else
            right_point_count++;
    }

    bool area_empty = (left_point_count + right_point_count) == 0;

    // 3 state: go, turn, break loop. eğer 5 kere hiç ileri gitmeden sağa sola dönüş yaptıysa lidar boşluk görene kadar tek bir yönde dönmeye devam eder.

    if(state_ == RobotState::GO_FORWARD)
    {
        if(!area_empty)
        {
            turn_direction_ = (left_point_count <= right_point_count) ? TURN_LEFT : TURN_RIGHT;
            state_ = RobotState::TURN;
        }
    }
    else if(state_ == RobotState::TURN)
    {
        if(area_empty)
            state_ = RobotState::GO_FORWARD;
        else
            // yön her taramada yeniden seçilir (osilasyonu FIFO yakalar)
            turn_direction_ = (left_point_count <= right_point_count) ? TURN_LEFT : TURN_RIGHT;
    }
    else // RobotState::BREAK_LOOP
    {
        // yön değiştirilmez, alan tamamen boşalana kadar aynı yöne dönülür
        if(area_empty)
        {
            state_ = RobotState::GO_FORWARD;
            motion_history_.clear();
        }
    }

    // hareketi kaydedilir ve sıkışmayı kontrol edilir
    if(state_ != RobotState::BREAK_LOOP)
    {
        Motion current_motion;
        if(state_ == RobotState::TURN && turn_direction_ == TURN_LEFT)
            current_motion = Motion::LEFT;
        else if(state_ == RobotState::TURN && turn_direction_ == TURN_RIGHT)
            current_motion = Motion::RIGHT;
        else
            current_motion = Motion::FORWARD;

        add_motion_to_history(current_motion);

        if(is_stuck_in_loop())
        {
            // FIFO'daki son hareketi lidarın önü açılana dek sürdürür.
            turn_direction_ = (motion_history_.back() == Motion::LEFT) ? TURN_LEFT : TURN_RIGHT;
            state_ = RobotState::BREAK_LOOP;
        }
    }

    // 4) hareket komutu
    geometry_msgs::msg::TwistStamped cmd_vel_msg;
    cmd_vel_msg.header.stamp = this->now();
    cmd_vel_msg.header.frame_id = "base_footprint";

    if(state_ == RobotState::GO_FORWARD)
    {
        cmd_vel_msg.twist.linear.x = LINEAR_SPEED_MPS;
        cmd_vel_msg.twist.angular.z = 0.0;
    }
    else
    {
        cmd_vel_msg.twist.linear.x = 0.0;
        cmd_vel_msg.twist.angular.z = turn_direction_ * ANGULAR_SPEED_RADPS;
    }

    cmd_vel_pub_->publish(cmd_vel_msg);

    RCLCPP_INFO(this->get_logger(), "left: %d  right: %d  state: %s",
                left_point_count, right_point_count,
                state_ == RobotState::GO_FORWARD ? "GO_FORWARD" :
                state_ == RobotState::TURN       ? "TURN" : "BREAK_LOOP");
}

void BumpAndGo::add_motion_to_history(Motion motion)
{
    // hareket değiştiği zaman geçmişe eklenir.
    if(!motion_history_.empty() && motion_history_.back() == motion)
        return;

    motion_history_.push_back(motion);

    if(motion_history_.size() > MOTION_HISTORY_SIZE)
        motion_history_.pop_front();
}

bool BumpAndGo::is_stuck_in_loop()
{
    // hareket geçmişi dolmamışsa sıkışma yok
    if(motion_history_.size() < MOTION_HISTORY_SIZE)
        return false;

    // son hareketlerden en az biri ileri gitmekse sıkışma yok
    for(Motion motion : motion_history_)
    {
        if(motion == Motion::FORWARD)
            return false;
    }
    return true;
}

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<BumpAndGo>());
    rclcpp::shutdown();
    return 0;
}
