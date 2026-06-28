#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi 凭据
const char* ssid = "shiqi";
const char* password = "000000000";

WebServer server(80);
const int ledPin = 2;
const int touchPin = 4;
const int threshold = 400; // 触摸阈值

bool isArmed = false;     // 布防状态
bool isAlarming = false;  // 报警锁定状态
unsigned long lastBlinkTime = 0;
bool ledState = false;

// 网页HTML：新增实时触摸数值仪表盘、AJAX定时拉取数据
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>ESP32 安防实时仪表盘</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { text-align: center; font-family: Arial; padding-top: 40px; }
    h1 {font-size:36px;}
    /* 实时数值仪表盘样式 */
    .dashboard {
      font-size: 80px;
      font-weight: bold;
      color: #222;
      margin: 40px 0;
      padding:20px;
      border:3px solid #666;
      border-radius:16px;
      max-width:400px;
      margin-left:auto;
      margin-right:auto;
    }
    .tip {font-size:18px; color:#555; margin-bottom:30px;}
    .state-text {font-size:24px; margin:20px 0;}
    button { font-size: 20px; padding: 15px 30px; margin: 10px; border-radius: 8px; cursor: pointer; }
    .arm { background-color: #f44336; color: white; border: none; }
    .disarm { background-color: #4CAF50; color: white; border: none; }
  </style>
</head>
<body>
  <h1>安防报警主机 - 实时触摸仪表盘</h1>
  <p class="state-text">当前状态: <strong id="status">未布防</strong></p>

  <!-- 实时触摸数值仪表盘 -->
  <p class="tip">触摸传感器实时数值（靠近引脚数值变小）</p>
  <div class="dashboard" id="touchNum">0</div>

  <button class="arm" onclick="sendCommand('arm')">布防 (Arm)</button>
  <button class="disarm" onclick="sendCommand('disarm')">撤防 (Disarm)</button>

  <script>
    // 发送布防/撤防指令
    function sendCommand(cmd) {
      fetch("/" + cmd).then(() => {
        document.getElementById("status").innerText = (cmd === 'arm') ? "已布防 (监控中)" : "未布防 (安全)";
      });
    }

    // 定时拉取触摸数值，每100ms刷新一次
    function getTouchValue() {
      fetch("/touchVal")
        .then(res => res.text())
        .then(val => {
          document.getElementById("touchNum").innerText = val;
        })
        .catch(err => console.log("数据拉取失败"));
    }
    // 每100毫秒自动更新仪表盘
    setInterval(getTouchValue, 100);
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nIP Address: " + WiFi.localIP().toString());

  // 1. 主页路由，返回完整网页
  server.on("/", []() {
    server.send(200, "text/html; charset=utf-8", htmlPage);
  });

  // 2. 新增接口：返回当前触摸传感器原始数值（供前端AJAX拉取）
  server.on("/touchVal", []() {
    int val = touchRead(touchPin);
    server.send(200, "text/plain", String(val));
  });

  // 布防接口
  server.on("/arm", []() {
    isArmed = true;
    server.send(200, "text/plain", "Armed");
    Serial.println("系统已布防！");
  });

  // 撤防接口，清除报警锁定、关闭LED
  server.on("/disarm", []() {
    isArmed = false;
    isAlarming = false;
    digitalWrite(ledPin, LOW);
    server.send(200, "text/plain", "Disarmed");
    Serial.println("系统已撤防！");
  });

  server.begin();
}

void loop() {
  server.handleClient(); // 持续处理网页、数据请求

  // 布防状态下检测触摸，触发后锁定报警
  if (isArmed && !isAlarming) {
    if (touchRead(touchPin) < threshold) {
      isAlarming = true;
      Serial.println("警报触发！");
    }
  }

  // 报警LED高频非阻塞闪烁
  if (isAlarming) {
    if (millis() - lastBlinkTime > 50) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      lastBlinkTime = millis();
    }
  }
}