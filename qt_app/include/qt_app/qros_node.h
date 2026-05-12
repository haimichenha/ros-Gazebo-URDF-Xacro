#ifndef QROS_NODE_H
#define QROS_NODE_H

#include <QThread>
#include <QImage>
#include <ros/ros.h>
#include <string>

#include <nav_msgs/Odometry.h>
#include <sensor_msgs/Image.h>
#include <geometry_msgs/Twist.h>
#include <image_transport/image_transport.h>

class QROSNode : public QThread
{
  Q_OBJECT

public:
  QROSNode();
  void run() override;

  // 公共接口函数
  void launch(const std::string& command);
  void stop();
  void publish_cmd_vel(double linear, double angular);

private:
  void init(); // <--- chongzhi gezhong yemmianshuju

  // 回调函数
  void odom_callback(const nav_msgs::Odometry::ConstPtr& msg);
  void camera_callback(const sensor_msgs::ImageConstPtr& msg);

Q_SIGNALS:
  void odometry_updated(double x, double y, double th, double vx, double wz);
  void camera_image_updated(const QImage &image);

private:
  // ROS句柄、发布者和订阅者
  ros::NodeHandle* nh_ptr_; // 使用指针，方便管理
  image_transport::ImageTransport* it_ptr_;
  ros::Subscriber odom_sub_;
  image_transport::Subscriber camera_sub_;
  ros::Publisher cmd_vel_pub_;

  bool is_initialized_; // <--- 新增：初始化状态标志
};

#endif
