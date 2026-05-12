#include <ros/ros.h>
#include <QApplication>
#include "qt_app/main_window.h"

int main(int argc, char *argv[])
{
  ros::init(argc, argv, "qt_app_node");
  
  QApplication a(argc, argv);
  
  MainWindow w;
  w.show();

  return a.exec();
}

