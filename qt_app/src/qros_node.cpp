// 文件: ~/catkin_ws/src/qt_app/src/qros_node.cpp (最终修复版)

#include "qt_app/qros_node.h"
#include <tf/transform_datatypes.h>
#include <cv_bridge/cv_bridge.h>
#include <cstdlib>
#include <ros/master.h>

QROSNode::QROSNode() : nh_ptr_(nullptr), it_ptr_(nullptr), is_initialized_(false) {}

void QROSNode::init() {
    if(is_initialized_) return;
    nh_ptr_ = new ros::NodeHandle();
    it_ptr_ = new image_transport::ImageTransport(*nh_ptr_);
    odom_sub_ = nh_ptr_->subscribe("/odom_rf2o", 10, &QROSNode::odom_callback, this);   //lichengji
    camera_sub_ = it_ptr_->subscribe("/camera/image_raw", 1, &QROSNode::camera_callback, this);
    cmd_vel_pub_ = nh_ptr_->advertise<geometry_msgs::Twist>("/cmd_vel", 1);   //ros TX
    is_initialized_ = true;
    ROS_INFO("QROSNode Initialized and subscribed to topics.");
}

void QROSNode::run() {
    // 【【【 核心修正：先等待 roscore 启动 】】】
    while(!ros::master::check()) {
        QThread::msleep(200); // 等待 roscore
    }

    // 只有在 roscore 存在后，才初始化第一个 NodeHandle 并进入主循环
    // 这会初始化 ROS Time 系统
    ros::NodeHandle n;
    ROS_INFO("Successfully connected to ROS Master.");

    ros::Rate loop_rate(5); // 现在创建 ros::Rate 是安全的了

    while(ros::ok()) {
        if (ros::master::check()) {
            if (!is_initialized_) {
                init();
            }
        } else {
            if (is_initialized_) {      //roscore
                ROS_WARN("Connection to ROS Master lost! Resetting subscribers.");
                odom_sub_.shutdown();
                camera_sub_.shutdown();
                cmd_vel_pub_.shutdown();
                delete it_ptr_;
                delete nh_ptr_;
                it_ptr_ = nullptr;
                nh_ptr_ = nullptr;
                is_initialized_ = false;
            }
        }

        if(is_initialized_) {
            ros::spinOnce();
        }

        loop_rate.sleep();
    }
    ROS_INFO("QROSNode run loop finished.");
}


void QROSNode::launch(const std::string& command) {    //launch_js
    stop();
    ROS_INFO("Executing command: %s", command.c_str());
    system((command + " &").c_str());
}

void QROSNode::stop() {
    ROS_INFO("Stopping all related processes...");
    system("killall -9 gzserver gzclient roslaunch rosmaster move_base amcl map_server rviz &");
    QThread::msleep(1500);
}

void QROSNode::publish_cmd_vel(double linear, double angular) {   //sd guanli
    if(is_initialized_ && cmd_vel_pub_) {
        geometry_msgs::Twist twist_msg;
        twist_msg.linear.x = linear;
        twist_msg.angular.z = angular;
        cmd_vel_pub_.publish(twist_msg);
    }
}

void QROSNode::camera_callback(const sensor_msgs::ImageConstPtr& msg) {  //sxt
    try {
        cv_bridge::CvImageConstPtr cv_ptr = cv_bridge::toCvShare(msg, sensor_msgs::image_encodings::RGB8);
        const cv::Mat &frame = cv_ptr->image;
        QImage image(frame.data, frame.cols, frame.rows, QImage::Format_RGB888);
        Q_EMIT camera_image_updated(image.copy());
    } catch (cv_bridge::Exception& e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
    }
}

void QROSNode::odom_callback(const nav_msgs::Odometry::ConstPtr& msg) {    //odem LC
    double x = msg->pose.pose.position.x;
    double y = msg->pose.pose.position.y;
    tf::Quaternion q(msg->pose.pose.orientation.x, msg->pose.pose.orientation.y, msg->pose.pose.orientation.z, msg->pose.pose.orientation.w);
    tf::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);
    double vx = msg->twist.twist.linear.x;
    double wz = msg->twist.twist.angular.z;
    Q_EMIT odometry_updated(x, y, yaw, vx, wz);
}
