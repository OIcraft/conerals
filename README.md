这是一个控制台版本的generals

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

编译完成后，运行main.exe即可

对于Windows 11用户无法点击按钮的问题：
在cmd中通过 `start /min main.exe` 启动
