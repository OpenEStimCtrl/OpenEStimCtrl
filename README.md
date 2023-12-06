# OpenEStimCtrl - 开放的电刺激控制协议库

![Version 0.0.1](https://img.shields.io/badge/Version-0.0.1-blue)

## 支持的设备
- [x] DG-Lab ESTIM01 地牢实验室 郊狼1代
- [x] YokoNex ES01 役次元1代

## 提供的功能
- [x] 构建 BLE 发送数据包
- [x] 解析设备的 BLE 回应并通过事件函数返回
- [x] 自动端序转换
- [x] C 绑定，可用于其他语言的 c-interop 或者 cffi 绑定
- [x] 提供静态链接和动态链接两种库模式
- [x] 波形生成计算

## 编译
本库使用 CMake 进行编译，编译前请确保已安装以下依赖项：
- CMake 3.25+
- C++17 编译器

无额外依赖项，直接编译即可。

## 协议
本库使用 GNU AFFERO GENERAL PUBLIC LICENSE Version 3 (AGPLv3) 协议开源

## 免责声明
OpenEStimCtrl 不对您使用相关设备发生的问题负责。

请遵循设备官方推荐安全使用设备

## 非盈利项目
本项目开发者不使用此项目盈利

只为能让相关设备有更多可能

关联项目：[OpenDGLab](opendglab.github.io)