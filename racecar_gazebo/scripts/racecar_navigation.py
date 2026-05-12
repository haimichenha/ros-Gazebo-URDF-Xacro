#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
纯净版导航指令翻译脚本 (V-Final)
职责：忠实地将 /cmd_vel (m/s, rad/s) 翻译成底层控制器需要的指令。
"""
import rospy
from std_msgs.msg import Float64
from geometry_msgs.msg import Twist

class CmdVelTranslator:
    def __init__(self):
        rospy.init_node('cmd_vel_translator', anonymous=True)

        # --- 从参数服务器读取换算系数，这是最佳实践 ---
        # 您可以根据实际表现，在launch文件中或通过rosparam set来调整它们
        self.speed_factor = rospy.get_param('~speed_factor', 25.0) # 将 m/s 转换成 speed 单位的系数
        self.turn_factor = rospy.get_param('~turn_factor', 1.0)   # 将 rad/s 转换成 turn 单位的系数

        rospy.loginfo(f"CmdVel Translator启动。速度系数: {self.speed_factor}, 转向系数: {self.turn_factor}")

        # --- 发布者与订阅者 ---
        self.speed_pub = rospy.Publisher("XL_control_cmd/speed", Float64, queue_size=1)
        self.turn_pub = rospy.Publisher("XL_control_cmd/turn", Float64, queue_size=1)
        self.cmd_vel_sub = rospy.Subscriber("cmd_vel", Twist, self.cmd_vel_callback)

    def cmd_vel_callback(self, twist_msg):
        # 接收 TEB 计算出的标准 Twist 消息
        # twist_msg.linear.x  是线速度 (m/s)
        # twist_msg.angular.z 是转向角或角速度 (rad/s 或 rad)
        # TEB在 cmd_angle_instead_rotvel=True 时，这里发布的是转向角(rad)

        linear_x = twist_msg.linear.x
        angular_z = twist_msg.angular.z

        # 直接进行线性换算
        # 注意：这里的换算系数需要根据您底层控制器的实际需求来确定。
        # 例如，如果底层控制器需要一个0-100的速度值，而TEB的最大速度是0.2m/s，
        # 那么 speed_factor 可能需要设置为 500 (100 / 0.2)。
        # 我们先用一个经验值，您可以根据实际表现来调整。
        speed_cmd = linear_x * self.speed_factor
        turn_cmd = angular_z * self.turn_factor

        # 发布给底层控制器
        self.speed_pub.publish(Float64(speed_cmd))
        self.turn_pub.publish(Float64(turn_cmd))

    def run(self):
        rospy.spin()

if __name__ == '__main__':
    try:
        translator = CmdVelTranslator()
        translator.run()
    except rospy.ROSInterruptException:
        pass
