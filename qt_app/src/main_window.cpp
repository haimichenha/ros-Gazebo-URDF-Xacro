#include "qt_app/main_window.h"
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QImage>
#include <sstream>
#include <QApplication>
#include <QThread>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setup_ui();
  connect(&qros_node_, &QROSNode::odometry_updated, this, &MainWindow::update_odometry_display);
  connect(&qros_node_, &QROSNode::camera_image_updated, this, &MainWindow::update_camera_display);
  connect(launch_button_, &QPushButton::clicked, this, &MainWindow::on_launchButton_clicked);
  connect(stop_button_, &QPushButton::clicked, this, &MainWindow::on_stopButton_clicked);
  connect(forward_button_, &QPushButton::pressed, this, &MainWindow::on_manual_control_button_pressed);
  connect(forward_button_, &QPushButton::released, this, &MainWindow::on_manual_control_button_released);
  connect(backward_button_, &QPushButton::pressed, this, &MainWindow::on_manual_control_button_pressed);
  connect(backward_button_, &QPushButton::released, this, &MainWindow::on_manual_control_button_released);
  connect(left_button_, &QPushButton::pressed, this, &MainWindow::on_manual_control_button_pressed);
  connect(left_button_, &QPushButton::released, this, &MainWindow::on_manual_control_button_released);
  connect(right_button_, &QPushButton::pressed, this, &MainWindow::on_manual_control_button_pressed);
  connect(right_button_, &QPushButton::released, this, &MainWindow::on_manual_control_button_released);
  qros_node_.start();
}
MainWindow::~MainWindow() {}

void MainWindow::reset_ui() {
    camera_view_label_->clear();
    camera_view_label_->setStyleSheet("background-color: black; border: 1px solid gray;");
    camera_view_label_->setText("正在处理，请稍候...");
    pos_x_edit_->setText("--"); pos_y_edit_->setText("--"); pos_theta_edit_->setText("--");
    vel_lin_x_edit_->setText("--"); vel_ang_z_edit_->setText("--");
    QApplication::processEvents();
}

void MainWindow::setup_ui() {
    setWindowTitle("机器人导航与控制系统");
    QWidget* central_widget = new QWidget(); setCentralWidget(central_widget); QHBoxLayout* main_layout = new QHBoxLayout(central_widget);
    QVBoxLayout* left_panel_layout = new QVBoxLayout();
    QGroupBox* mode_group = new QGroupBox("运行模式"); QVBoxLayout* mode_layout = new QVBoxLayout(mode_group);
    manual_mode_radio_ = new QRadioButton("手动驾驶模式"); auto_mode_radio_ = new QRadioButton("自主导航模式");
    mode_layout->addWidget(manual_mode_radio_); mode_layout->addWidget(auto_mode_radio_);
    auto_mode_radio_->setChecked(true); left_panel_layout->addWidget(mode_group);

    QGroupBox* control_group = new QGroupBox("导航参数设置"); QGridLayout* control_layout = new QGridLayout(control_group);
    scene_combo_box_ = new QComboBox();
    scene_combo_box_->addItems({"默认跑道 (Runway)", "砖墙迷宫 (Brick World)", "木头世界 (Wood World)"});
    algo_combo_box_ = new QComboBox(); algo_combo_box_->addItems({"DWA Planner", "TEB Planner"});
    launch_button_ = new QPushButton("启动 / 切换"); stop_button_ = new QPushButton("停止所有进程");

    control_layout->addWidget(new QLabel("选择场景:"), 0, 0); control_layout->addWidget(scene_combo_box_, 0, 1);
    control_layout->addWidget(new QLabel("选择算法:"), 1, 0); control_layout->addWidget(algo_combo_box_, 1, 1);
    control_layout->addWidget(launch_button_, 2, 0, 1, 2); control_layout->addWidget(stop_button_, 3, 0, 1, 2);
    left_panel_layout->addWidget(control_group);

    QGroupBox* manual_group = new QGroupBox("手动控制"); QGridLayout* manual_layout = new QGridLayout(manual_group);
    forward_button_ = new QPushButton("↑"); backward_button_ = new QPushButton("↓"); left_button_ = new QPushButton("←"); right_button_ = new QPushButton("→");
    manual_layout->addWidget(forward_button_, 0, 1); manual_layout->addWidget(left_button_, 1, 0); manual_layout->addWidget(backward_button_, 1, 1); manual_layout->addWidget(right_button_, 1, 2);
    left_panel_layout->addWidget(manual_group); left_panel_layout->addStretch();

    camera_view_label_ = new QLabel("等待摄像头画面..."); camera_view_label_->setMinimumSize(640, 480);
    camera_view_label_->setStyleSheet("background-color: black; border: 1px solid gray;"); camera_view_label_->setAlignment(Qt::AlignCenter);
    QVBoxLayout* right_panel_layout = new QVBoxLayout(); QGroupBox* odom_group = new QGroupBox("车辆状态");
    QGridLayout* odom_layout = new QGridLayout(odom_group);

    pos_x_edit_ = new QLineEdit("--"); pos_x_edit_->setReadOnly(true); pos_y_edit_ = new QLineEdit("--"); pos_y_edit_->setReadOnly(true);
    pos_theta_edit_ = new QLineEdit("--"); pos_theta_edit_->setReadOnly(true); vel_lin_x_edit_ = new QLineEdit("--"); vel_lin_x_edit_->setReadOnly(true);
    vel_ang_z_edit_ = new QLineEdit("--"); vel_ang_z_edit_->setReadOnly(true);
    odom_layout->addWidget(new QLabel("X坐标 (m):"), 0, 0); odom_layout->addWidget(pos_x_edit_, 0, 1);
    odom_layout->addWidget(new QLabel("Y坐标 (m):"), 1, 0); odom_layout->addWidget(pos_y_edit_, 1, 1);
    odom_layout->addWidget(new QLabel("朝向 (rad):"), 2, 0); odom_layout->addWidget(pos_theta_edit_, 2, 1);
    odom_layout->addWidget(new QLabel("线速度 (m/s):"), 3, 0); odom_layout->addWidget(vel_lin_x_edit_, 3, 1);
    odom_layout->addWidget(new QLabel("角速度 (rad/s):"), 4, 0); odom_layout->addWidget(vel_ang_z_edit_, 4, 1);
    right_panel_layout->addWidget(odom_group); right_panel_layout->addStretch();
    main_layout->addLayout(left_panel_layout); main_layout->addWidget(camera_view_label_, 1); main_layout->addLayout(right_panel_layout);
}

void MainWindow::on_launchButton_clicked() {
    reset_ui();
    qros_node_.stop();
    QThread::msleep(1000);

    std::ostringstream command;
    QString selected_scene = scene_combo_box_->currentText();
    bool is_manual_mode = manual_mode_radio_->isChecked();
    std::string world_file, x_pos, y_pos, map_file;


    if (selected_scene.contains("Runway")) {             //ditu
        world_file = "$(find racecar_gazebo)/worlds/racecar_runway.world";
        map_file = "$(find racecar_gazebo)/maps/map.yaml"; // 或者 map_hh.yaml
        x_pos = "-0.5";
        y_pos = "-0.05";
    } else if (selected_scene.contains("Brick")) {
        world_file = "$(find racecar_gazebo)/worlds/brick_world.world";
        map_file = "$(find racecar_gazebo)/maps/brick_map.yaml";
        x_pos = "-6.9";
        y_pos = "-1.2";
    } else if (selected_scene.contains("Wood")) {
        world_file = "$(find racecar_gazebo)/worlds/wood_map.world";
        map_file = "$(find racecar_gazebo)/maps/wood_map.yaml";
        x_pos = "-6.9";
        y_pos = "-6.0";
    }

    if (is_manual_mode) {

        command << "roslaunch racecar_gazebo manual_drive.launch "
                << "world_file:='" << world_file << "' "
                << "x_pos:=" << x_pos << " "
                << "y_pos:=" << y_pos;
    } else {
        // 【【【 核心修正：自主导航模式的逻辑也是统一的！ 】】】
        // 不再需要特殊判断，直接调用通用的 autonomous_nav.launch 即可
        QString selected_algo = algo_combo_box_->currentText();
        std::string local_planner, planner_config_file;

        if (selected_algo.contains("DWA")) {
            local_planner = "dwa_local_planner/DWAPlannerROS";
            planner_config_file = "$(find racecar_gazebo)/config/navigation/dwa_local_planner_params.yaml";
        } else {
            local_planner = "teb_local_planner/TebLocalPlannerROS";
            planner_config_file = "$(find racecar_gazebo)/config/navigation/teb_local_planner_params.yaml";
        }

        command << "roslaunch racecar_gazebo autonomous_nav.launch "
                << "world_file:='" << world_file << "' "
                << "map_file:='" << map_file << "' "  // <-- 把正确的地图文件传进去
                << "x_pos:=" << x_pos << " "
                << "y_pos:=" << y_pos << " "
                << "local_planner:='" << local_planner << "' "
                << "planner_config_file:='" << planner_config_file << "'";
    }

    qros_node_.launch(command.str());
}

//
void MainWindow::on_stopButton_clicked() {
    reset_ui();
    qros_node_.stop();
}
void MainWindow::update_camera_display(const QImage &image) { camera_view_label_->setPixmap(QPixmap::fromImage(image).scaled(camera_view_label_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); }
void MainWindow::update_odometry_display(double x, double y, double th, double vx, double wz) {
    pos_x_edit_->setText(QString::number(x, 'f', 2)); pos_y_edit_->setText(QString::number(y, 'f', 2));
    pos_theta_edit_->setText(QString::number(th, 'f', 2)); vel_lin_x_edit_->setText(QString::number(vx, 'f', 2));
    vel_ang_z_edit_->setText(QString::number(wz, 'f', 2));
}
void MainWindow::on_manual_control_button_pressed() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button == forward_button_)   qros_node_.publish_cmd_vel(0.5, 0.0);
    else if (button == backward_button_) qros_node_.publish_cmd_vel(-0.5, 0.0);
    else if (button == left_button_)     qros_node_.publish_cmd_vel(0.2, 1.0);
    else if (button == right_button_)    qros_node_.publish_cmd_vel(0.2, -1.0);
}
void MainWindow::on_manual_control_button_released() {
    qros_node_.publish_cmd_vel(0.0, 0.0);
}
