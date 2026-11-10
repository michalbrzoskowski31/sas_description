cmake_minimum_required(VERSION 3.8)
project(sas_description)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()


# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(joint_controller 
  src/joint_controller.cpp
)

ament_target_dependencies(
  joint_controller
  rclcpp
  sensor_msgs
  std_msgs
)

#klawaitura
add_executable(teleop_keyboard src/teleop_keyboard.cpp)
ament_target_dependencies(
  teleop_keyboard
  rclcpp
  std_msgs
)


install(TARGETS
  joint_controller
  teleop_keyboard
  DESTINATION lib/${PROJECT_NAME}
)


install(
  DIRECTORY urdf meshes rviz launch
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
