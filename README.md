# 超级联动日程表

##  项目需求

现如今大学生的大学生活总是非常丰富的，任务也多种多样，不仅有学习任务，也有平时社团的任务，多种多样的任务令人应接不暇，而完成任务的期限也各有不同。为了减少大家记忆的负担，并且与学校布置的课程任务相结合，本项目采用`Qt`与`canvas`连接，用`canvas`中已有的待办事项日历进行任务的记录，将任务表与大学网站相连，实现随时随地进行查询的功能。同时实现了在本地使用`Qt`产生图形化界面以及进行对任务的查询、添加和删除，且该修改功能不仅可以修改当前本地界面，也可以修改`canvas`上的日程表，实现同步修改功能。

## 功能要求

1. 使用`Qt`模拟登录通过统一身份认证链接至 http://canvas.tongji.edu.cn 的连接。
2. 获取`canvas`上所有的任务信息并进行解析。
3. 与`canvas`进行数据交互完成事件的添加和删除。
4. 实现通过下拉框的转换进行当天不同ddl在本地的显示

## 功能描述

本地登录需要进行用户名和密码的获取，本项目通过登录弹窗以及确认按钮实现用户名和密码的获取以及参数的传输，其实现置于`login.h`以及 `login.cpp` 中。

其余实现置于`mainwindow.h`以及`mainwindow.cpp`中。

网页登陆界面需要输入三种内容，包括用户名、密码以及验证码。其中，验证码从统一身份认证处获得，其余均为用户在本地窗口进行输入。首先获取验证码图片，使用机器识别算法获得验证码文本，然后将其封装为`json`对象并将所有信息使用`post`方法上传进行登录。

点击日历时显示当前日期所需要做完的任务并通过下拉框的选择进行当前日期不同任务的切换

通过`add`按钮可以进行`ddl`的添加。

通过`del`按钮可以实现`ddl`的删除。