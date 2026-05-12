# ROS/Gazebo 小车建图与导航仿真实践

> 面向简历展示整理的 ROS 课程/个人实践项目。项目基于 Gazebo 仿真小车模型，完成从车辆模型、传感器仿真、建图、定位到导航规划的基础链路搭建，并补充 Unity 语义地图标注数据用于说明自动驾驶地图数据准备流程。

## 项目简介

本项目围绕 `racecar` 小车仿真平台展开，主要实践内容包括：

- 使用 URDF/Xacro 描述小车模型，并在 Gazebo 中加载车辆、轮子、转向、雷达、相机、IMU 等组件；
- 使用 `rf2o_laser_odometry` 根据激光雷达数据估计里程计；
- 使用 `gmapping` / `hector_mapping` 进行建图实验；
- 使用 `map_server`、`AMCL`、`move_base` 完成定位与导航流程；
- 对比/配置 DWA、TEB 局部规划器和 costmap 参数；
- 通过 RViz 观察地图、雷达、路径规划和车辆状态；
- 使用 Qt/ROS 节点进行模式选择、启动控制、图像显示和里程计状态显示；
- 整理 Unity 语义地图标注导出的 `lane.csv`、`dtlane.csv`、`roadedge.csv` 和点云地图 `auto.pcd`。

## 技术栈

- Ubuntu 20.04 / ROS Noetic（原始环境为 Ubuntu 20.04 虚拟机）
- Gazebo、RViz、URDF/Xacro
- `robot_state_publisher`、`gazebo_ros_control`
- `rf2o_laser_odometry`
- `gmapping`、`hector_mapping`
- `map_server`、`AMCL`、`move_base`
- DWA Planner、TEB Local Planner、costmap_2d
- Python / C++ / Qt / ROS topic
- Unity 语义地图标注数据、PCD 点云地图

## 目录结构

```text
.
├── racecar_description/      # 小车 URDF/Xacro、传感器模型和 mesh 资源
├── racecar_gazebo/           # Gazebo world、launch、地图、导航参数、RViz 配置和控制脚本
├── rf2o_laser_odometry/      # 激光雷达里程计功能包
├── simple_layers/            # costmap 自定义 layer 示例
├── qt_app/                   # Qt + ROS 控制界面实践
└── docs/data/semantic_map/   # Unity 语义地图标注数据和 auto.pcd
```

## 主要功能

### 1. Gazebo 小车仿真

- `racecar_description` 定义车辆模型、轮子、转向结构和传感器；
- `racecar_gazebo/launch/racecar.launch` 启动 Gazebo 世界、加载 URDF 模型和控制器；
- `XL_controller_cmd.py` 将速度/转向指令转换为 Gazebo 控制器命令；
- `XL_keyboard_remote.py` 支持键盘遥控测试。

### 2. 建图实验

- `gmapping.launch`：使用 `slam_gmapping` 根据 `/scan` 进行栅格地图构建；
- `hector_demo.launch`：使用 hector mapping 进行建图对比；
- `remap_brick_world.launch`、`slam_world.launch` 等用于不同 world 场景下的建图实验。

### 3. 定位与导航

- `racecar_navigation.launch`：默认导航流程，包含 Gazebo、map_server、AMCL、move_base、RViz；
- `navigation_only.launch`：只启动导航相关组件；
- `autonomous_nav.launch`：支持通过参数传入地图、局部规划器和规划参数；
- `racecar_navigation.py` 将 `/cmd_vel` 转换为底层控制话题 `XL_control_cmd/speed` 和 `XL_control_cmd/turn`。

### 4. 语义地图数据

`docs/data/semantic_map/` 保存 Unity 标注导出的自动驾驶相关数据：

| 文件 | 说明 |
| --- | --- |
| `lane.csv` | 车道拓扑/车道连接信息 |
| `dtlane.csv` | 车道中心线采样点及方向、坡度等属性 |
| `roadedge.csv` | 道路边界/路沿关联信息 |
| `auto.pcd` | 点云地图数据，用于说明标注和地图准备流程 |

> 这些数据用于展示语义地图标注与导航数据准备流程，实际运行时需要根据本机 ROS/Unity 环境调整路径和转换脚本。

## 编译方式

建议把本仓库放入 catkin 工作空间的 `src` 下：

```bash
mkdir -p ~/catkin_ws/src
cd ~/catkin_ws/src
git clone https://github.com/haimichenha/ros-Gazebo-URDF-Xacro.git
cd ~/catkin_ws
catkin_make
source devel/setup.bash
```

## 运行示例

```bash
# 启动 Gazebo 小车仿真
roslaunch racecar_gazebo racecar.launch

# 键盘手动驾驶
roslaunch racecar_gazebo manual_drive.launch

# gmapping 建图演示
roslaunch racecar_gazebo gmapping_demo.launch

# 默认导航演示
roslaunch racecar_gazebo racecar_navigation.launch
```

> 如果地图、world 或资源路径在本机不存在，请优先检查 `racecar_gazebo/maps/`、`racecar_gazebo/worlds/` 和 launch 文件中的参数。

## 当前完成度

- 已完成 ROS/Gazebo 小车模型加载、传感器仿真和基础控制链路；
- 已完成建图、定位、导航规划链路的课程实践；
- 已配置 DWA/TEB、costmap、AMCL 等常见导航参数；
- 已整理 Unity 语义地图标注导出的 CSV 与 PCD 数据；
- 项目仍以课程/个人实践为主，未达到完整工程化自动驾驶系统标准。

## 后续优化

- 将 launch 文件中的绝对路径进一步参数化；
- 补充更完整的运行截图、RViz/Gazebo 效果图和演示视频；
- 对比 DWA 与 TEB 的导航效果并记录调参过程；
- 将 Unity 语义地图数据转换流程整理成独立脚本；
- 清理大体积地图/模型资源，必要时迁移到 Git LFS 或网盘说明。

## 简历描述参考

> ROS/Gazebo 小车建图与导航仿真实践：基于 ROS、Gazebo 和 RViz 搭建 racecar 小车仿真环境，完成 URDF/Xacro 模型配置、激光雷达/相机/IMU 传感器仿真、rf2o 激光里程计、gmapping/hector 建图、AMCL 定位和 move_base 导航流程；配置 DWA/TEB、costmap 等参数，并整理 Unity 语义地图标注数据用于自动驾驶地图准备流程展示。
