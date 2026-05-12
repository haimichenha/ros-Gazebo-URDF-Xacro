#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include <QMainWindow>
#include "qt_app/qros_node.h"
class QLabel; class QPushButton; class QLineEdit; class QComboBox; class QGroupBox; class QGridLayout; class QVBoxLayout; class QHBoxLayout; class QRadioButton;
class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
public Q_SLOTS:
  void update_odometry_display(double x, double y, double th, double vx, double wz);
  void update_camera_display(const QImage &image);
private Q_SLOTS:
  void on_launchButton_clicked();
  void on_stopButton_clicked();
  void on_manual_control_button_pressed();
  void on_manual_control_button_released();
private:
  void setup_ui();
  void reset_ui();
  QROSNode qros_node_;
  QRadioButton* manual_mode_radio_; QRadioButton* auto_mode_radio_; QLabel* camera_view_label_; QComboBox* scene_combo_box_; QComboBox* algo_combo_box_; QPushButton* launch_button_; QPushButton* stop_button_;
  QLineEdit* pos_x_edit_; QLineEdit* pos_y_edit_; QLineEdit* pos_theta_edit_; QLineEdit* vel_lin_x_edit_; QLineEdit* vel_ang_z_edit_;
  QPushButton* forward_button_; QPushButton* backward_button_; QPushButton* left_button_; QPushButton* right_button_;
};
#endif
