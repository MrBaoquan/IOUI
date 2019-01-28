# IOUI
Plugins of IOToolkit

# IOToolkit 插件开发
1.  克隆本项目到本地
2. 打开本项目，添加新建项目 NewIODev
3. 添加项目平台配置项, 选择Props文件夹下[配置项](https://github.com/MrBaoquan/IOUI/tree/master/Props "默认配置项")文件
4. 拷贝[IOUI](https://github.com/MrBaoquan/IOUI/tree/master/IOUI)下Source文件夹到 NewIODev 根目录下
5. 编写[IOUI.cpp](https://github.com/MrBaoquan/IOUI/blob/master/IOUI/Source/Private/IOUI.cpp)相应的函数实现
6. 编写[release.cmd](https://github.com/MrBaoquan/IOUI/blob/master/IOUI/Source/release.cmd) dos命令用于 将IOUI-[*].dll 复制到IOToolkit 目录下
7. 构建项目得到 IOUI-Win[36][24]-NewIODev.dll 文件，拷贝至IOToolkit插件位置即可
8. 使用
`<Device Name="NewIODev" Type="External" DllName="NewIODev" Index="0"></Device>`
