#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/bool.hpp>
#include <pigpio.h>
#include "servo.h"

const int servoPin = 37; // Change this to your Raspberry Pi's actual GPIO pin
const int angleTrue = 160;
const int angleFalse = 105;

class ServoControlNode : public rclcpp::Node {
public:
    ServoControlNode() : Node("servo_control_node"), servo(servoPin) {
        subscription_ = this->create_subscription<std_msgs::msg::Bool>(
            "servo_control", 10, std::bind(&ServoControlNode::topic_callback, this, std::placeholders::_1));
        
        // Initialize the servo to the default position
        servo.setAngle(angleFalse);
    }

private:
    void topic_callback(const std_msgs::msg::Bool::SharedPtr msg) {
        int angle = msg->data ? angleTrue : angleFalse;
        servo.setAngle(angle);
    }

    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr subscription_;
    Servo servo;
};

int main(int argc, char **argv) {
    if (gpioInitialise() < 0) {
        fprintf(stderr, "Failed to initialize pigpio\n");
        return 1;
    }

    rclcpp::init(argc, argv);
    auto node = std::make_shared<ServoControlNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();

    gpioTerminate(); // Cleanup pigpio
    return 0;
}