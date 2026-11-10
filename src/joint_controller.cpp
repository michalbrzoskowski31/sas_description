#include <chrono>

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>

using namespace std::chrono_literals;

class JointController : public rclcpp::Node {
public:
    JointController() : Node("joint_controller"), position_(0.0) {
        publisher_ = this->create_publisher
            <sensor_msgs::msg::JointState>
            ("/joint_states", 10);

        timer_ = this->create_wall_timer(
            20ms,
            std::bind(&JointController::timer_callback, this)
        );

        position_min_ = -0.464;
        position_max_ = 2.033;
        step_ = 0.02;
    }

private:
    void timer_callback() {
        auto msg = sensor_msgs::msg::JointState();
        msg.header.stamp = this->get_clock()->now();
        msg.name = {"carriage_joint"};

        position_ += step_;
        if (position_ > position_max_) {
            position_ = position_max_;
            step_ *= -1;
        }
        else if (position_ < position_min_) {
            position_ = position_min_;
            step_ *= -1;
        }

        msg.position = {position_};
        publisher_->publish(msg);
    }

    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    double position_;
    double position_max_;
    double position_min_;
    double step_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<JointController>();
    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}