# stm32-esp8266-mqtt
一、简介
1. 这是一个基于 HAL 库 + FreeRTOS + MQTT 的 STM32F103 环境监测系统，通过 ESP8266 与 MQTT 服务器（OneNET）通信

二、功能
1. 每 2 秒采集一次：
   - DHT11：温度、湿度
   - 光敏传感器：光照

2. ESP8266 与 MQTT 服务器通信：
   - ESP8266 上报属性（湿度、温度、光照强度、板载 LED 状态）到 MQTT 服务器
   - ESP8266 接收 MQTT 服务器下发的指令，控制板载 LED 的亮灭

3. 按键1切换 OLED 显示画面：
   - 画面1：湿度（当前 / 最大 / 最小）
   - 画面2：温度（当前 / 最大 / 最小）
   - 画面3：光照强度（当前 / 最大 / 最小）

4. 按键2控制板载 LED 的亮灭

5. LED 报警：
   - 湿度 > 阈值 → 红灯亮
   - 温度 > 阈值 → 绿灯亮
   - 光照强度 < 阈值 → 蓝灯亮

6. 串口（CH340）输出调试信息

三、硬件接线
1. DHT11：
   - VCC → 3.3V
   - GND → GND
   - DATA → PB12

2. 光敏传感器：
   - VCC → 3.3V
   - GND → GND
   - AO → PA4
   - DO → 悬空

3. ESP-01 (ESP8266)：
   - 3V3 → 3.3V
   - EN → 3.3V
   - GND → GND
   - TX → PB11
   - RX → PB10
   - RST → PB13
   - IO0 → 悬空
   - IO2 → 悬空

4. 按键1：
   - 一端 → 3.3V
   - 另一端 → PA6

5. 按键2：
   - 一端 → 3.3V
   - 另一端 → PA7

6. 0.96 寸 OLED：
   - VDD → 3.3V
   - GND → GND
   - SCK → PB8
   - SDA → PB9

7. LED：
   - 红灯正极 → 3.3V，负极 → PA1
   - 绿灯正极 → 3.3V，负极 → PA2
   - 蓝灯正极 → 3.3V，负极 → PA3

8. USB 转串口 CH340：
   - VCC → 3V3（短接）
   - GND → GND
   - TXD → PA10
   - RXD → PA9

四、软件配置
1. 报警阈值（APP\led\app_led.c）
   - HUMIDITY_THRESHOLD：湿度阈值
   - TEMPERATURE_THRESHOLD：温度阈值
   - LIGHT_PERCENTAGE_THRESHOLD：光照强度阈值

2. Wi-Fi 和 MQTT 服务器（APP\esp8266\app_esp8266.c）
   - ESP8266_AP_SSID：Wi-Fi 名称
   - ESP8266_AP_PWD：Wi-Fi 密码
   - CONNECT_ID：连接ID
   - MQTT_SERVER_IP：MQTT 服务器 IP
   - MQTT_SERVER_PORT：MQTT 服务器端口
   - DEVICE_NAME：设备名称
   - PRODUCT_ID：产品ID
   - TOKEN：token
   - MQTT_TOPIC_POST：设备上报属性的主题
   - MQTT_TOPIC_POST_REPLY：MQTT 服务器回复上报结果的主题
   - MQTT_TOPIC_SET：MQTT 服务器下发指令的主题
   - MQTT_TOPIC_SET_REPLY：设备回复指令执行结果的主题

五、设备属性
1. 湿度：
   - 标识符：humidity
   - 读写类型：只读
   - 是否必选：否
   - 数据类型：float
   - 取值范围：0.0-100.0
   - 步长：0.1
   - 单位：相对湿度 / %RH

2. 温度：
   - 标识符：temperature
   - 读写类型：只读
   - 是否必选：否
   - 数据类型：float
   - 取值范围：-100.0-100.0
   - 步长：0.1
   - 单位：摄氏度 / °C

3. 光照强度：
   - 标识符：light
   - 读写类型：只读
   - 是否必选：否
   - 数据类型：float
   - 取值范围：0.00-100.00
   - 步长：0.01
   - 单位：百分比 / %

4. 板载 LED：
   - 标识符：LED
   - 读写类型：读写
   - 是否必选：否
   - 数据类型：bool
   - 取值范围：true / false

六、使用说明
1. 按硬件接线说明连接好各模块

2. 打开手机热点，运行手机端 APP

3. 修改 APP\esp8266\app_esp8266.c 中的 Wi-Fi 和 MQTT 服务器信息

4. 根据需要修改 APP\led\app_led.c 中的报警阈值

5. 编译下载到 STM32

6. 打开串口助手查看调试信息

七、注意事项
1. ESP8266 原生固件的 AT 指令集可能不支持 MQTT 协议，需要将固件更新为支持 MQTT 协议的版本

2. 注意 token 的有效时间，如果 token 已过期，则需要更新 token
