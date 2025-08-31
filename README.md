这是一个控制台版本的 generals.io

编译注意事项：

系统环境要求：Windows 10/11

需要编译：
```
main.cpp
mapcreator.cpp
conerals_server\client.cpp
conerals_client\generals.cpp
conerals_client\main.cpp
conerals_client\generals_server.cpp
conerals_server\main_server.cpp
```

编译选项：`-std=c++17 -D _WIN32_WINNT 0x0A00 -lws2_32`

编译完成后，运行 main.exe 即可

编辑地图也可以直接用 mapcreator.exe 打开 .cmap 文件

最好将 mapcreator.exe 设为打开 .cmap 文件的默认值

对于 Windows 11 用户无法点击按钮的问题：

1. 在 cmd 中通过 `start /min main.exe` 启动
2. 将“默认终端应用程序”改为“Windows控制台主机”
