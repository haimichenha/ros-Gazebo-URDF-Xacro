# 文件: qt_app/CMakeLists.txt (最终修正版，明确处理MOC)

# --- 0. 基本设置 ---
cmake_minimum_required(VERSION 3.0.2)
project(qt_app)

# --- 1. 查找所有必需的包 ---
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(OpenCV REQUIRED)
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  nav_msgs
  geometry_msgs
  tf
  image_transport
  cv_bridge
)

# --- 2. 【【【 核心修改：明确处理 MOC 】】】 ---
# 我们不再依赖 CMAKE_AUTOMOC，而是手动指定需要MOC处理的头文件。
# 这种方法更明确，也更可靠。
qt5_wrap_cpp(MOC_FILES
  include/qt_app/main_window.h
  include/qt_app/qros_node.h
)

# --- 3. 定义Catkin包信息 ---
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS roscpp std_msgs nav_msgs geometry_msgs tf image_transport cv_bridge
)

# --- 4. 添加头文件搜索路径 ---
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  # 【【【 核心修改：添加MOC文件的输出目录 】】】
  # MOC生成的文件会被放在编译目录中，必须将此目录包含进来
  ${CMAKE_CURRENT_BINARY_DIR}
)

# --- 5. 定义可执行文件 ---
# 【【【 核心修改：将MOC生成的文件添加到源文件列表 】】】
add_executable(${PROJECT_NAME}_node
  src/main.cpp
  src/main_window.cpp
  src/qros_node.cpp
  ${MOC_FILES}
)

# --- 6. 链接所有需要的库 ---
target_link_libraries(${PROJECT_NAME}_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
)

# --- 7. 添加安装规则 ---
install(TARGETS ${PROJECT_NAME}_node
  RUNTIME DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
)


