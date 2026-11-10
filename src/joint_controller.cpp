#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/float64.hpp"

using namespace std::chrono_literals;

class JointController : public rclcpp::Node {
public:
  JointController()
  : Node("joint_controller"),
    position_(0.0),
    current_velocity_(0.0),
    position_min_(-0.464),
    position_max_(2.033)
  {
    // Publisher stanu stawów
    joint_state_pub_ = this->create_publisher<sensor_msgs::msg::JointState>("/joint_states", 10);

    // Subscriber prędkości z klawiatury (lub innego źródła)
    cmd_sub_ = this->create_subscription<std_msgs::msg::Float64>(
      "/cmd_carriage_vel", 10,
      [this](const std_msgs::msg::Float64::SharedPtr msg) {
        current_velocity_ = msg->data;
      });

    // Pętla integracji pozycji (50 Hz = co 20 ms)
    timer_ = this->create_wall_timer(
      20ms, std::bind(&JointController::timer_callback, this));

    RCLCPP_INFO(this->get_logger(), "JointController gotowy. Czekam na /cmd_carriage_vel...");
  }

private:
  void timer_callback()
  {
    const double dt = 0.02; // 20 ms
    position_ += current_velocity_ * dt;

    // Ograniczenie limitami
    if (position_ > position_max_) {
      position_ = position_max_;
    } else if (position_ < position_min_) {
      position_ = position_min_;
    }

    auto msg = sensor_msgs::msg::JointState();
    msg.header.stamp = this->get_clock()->now();
    msg.name = {"carriage_joint"};
    msg.position = {position_};

    joint_state_pub_->publish(msg);
  }

  rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr joint_state_pub_;
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr cmd_sub_;
  rclcpp::TimerBase::SharedPtr timer_;

  double position_;
  double current_velocity_;
  double position_min_;
  double position_max_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<JointController>());
  rclcpp::shutdown();
  return 0;
}