#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

// Funkcja do czytania pojedynczego znaku bez konieczności klikania Enter
char getch()
{
  char buf = 0;
  struct termios old{};
  if (tcgetattr(0, &old) < 0) {
    perror("tcsetattr()");
  }
  old.c_lflag &= ~ICANON; // wyłącz buforowanie linii
  old.c_lflag &= ~ECHO;   // wyłącz wypisywanie znaku na ekran
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSANOW, &old) < 0) {
    perror("tcsetattr ICANON");
  }
  if (read(0, &buf, 1) < 0) {
    perror("read()");
  }
  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  if (tcsetattr(0, TCSADRAIN, &old) < 0) {
    perror("tcsetattr ~ICANON");
  }
  return buf;
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("teleop_keyboard");
  auto pub = node->create_publisher<std_msgs::msg::Float64>("/cmd_carriage_vel", 10);

  std::cout << "=======================================\n";
  std::cout << "  Sterowanie wiertlem z klawiatury:    \n";
  std::cout << "  W - jedz w gore                      \n";
  std::cout << "  S - jedz w dol                       \n";
  std::cout << "  SPACJA - zatrzymaj                   \n";
  std::cout << "  Q - wyjdz                            \n";
  std::cout << "=======================================\n";

  const double speed = 1.0; // m/s
  bool running = true;

  while (rclcpp::ok() && running) {
    char c = getch();
    auto msg = std_msgs::msg::Float64();

    if (c == 'w' || c == 'W') {
      msg.data = speed;
      std::cout << "\rRuch: GÓRA  (" << speed << " m/s)" << std::flush;
    } else if (c == 's' || c == 'S') {
      msg.data = -speed;
      std::cout << "\rRuch: DÓŁ   (" << -speed << " m/s)" << std::flush;
    } else if (c == ' ') {
      msg.data = 0.0;
      std::cout << "\rRuch: STOP                     " << std::flush;
    } else if (c == 'q' || c == 'Q') {
      msg.data = 0.0;
      pub->publish(msg);
      running = false;
      std::cout << "\nKoniec teleoperacji.\n";
      break;
    }

    pub->publish(msg);
  }

  rclcpp::shutdown();
  return 0;
}