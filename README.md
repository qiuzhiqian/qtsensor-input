# qtsensor-input
这是一个基于input子系统驱动的qtsensor工程，主要实现了qtsensor对接input sensor的插件，是的上层能够直接使用qtsensor来访问底层sensor.

## 编译
先安装一下编译相关的依赖“
```
sudo apt install qtbase5-dev libqt5sensors5-dev
```
然后执行如下流程进行编译：
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## 安装
该工程会编译出来一个动态库插件```libqtsensor_input.so```和一个应用程序```sensor-app```。
- ```libqtsensor_input.so```安装到```/usr/lib/aarch64-linux-gnu/qt5/plugins/sensors/```
- ```sensor-app```是可执行程序，放到/usr/bin目录即可。

## 运行
```
# QT_LIGHT_INPUT_PATH=/dev/input/event1 QT_ACCEL_INPUT_PATH=/dev/input/event2 sensor-app
start...
code= 0  value= -80
onReadVal x= -80 y= 0 z= 0
code= 1  value= -352
onReadVal x= -80 y= -352 z= 0
code= 2  value= 16720
onReadVal x= -80 y= -352 z= 16720
code= 1  value= -368
onReadVal x= -80 y= -368 z= 16720
code= 2  value= 16672
onReadVal x= -80 y= -368 z= 16672
code= 0  value= -112
onReadVal x= -112 y= -368 z= 16672
code= 1  value= -352
onReadVal x= -112 y= -352 z= 16672
code= 2  value= 16736
onReadVal x= -112 y= -352 z= 16736
code= 0  value= -64
onReadVal x= -64 y= -352 z= 16736
code= 1  value= -384
onReadVal x= -64 y= -384 z= 16736
```