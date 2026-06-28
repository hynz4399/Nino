#include <WiFi.h>
#include <WebServer.h>

// 已配置的 Wi-Fi 凭据
const char* ssid = "shiqi";
const char* password = "000000000";

WebServer server(80);
const int ledPin = 2;
const int touchPin = 4;
const int threshold = 400; // 触摸阈值

bool isArmed = false;     // 系统是否处于布防状态
bool isAlarming = false;  // 系统是否正在报警

unsigned long lastBlinkTime = 0; // 用于非阻塞闪烁
bool ledState = false;

// 网页 HTML
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>ESP32 安防系统</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { text-align: center; font-family: Arial; padding-top: 50px; }
    button { font-size: 20px; padding: 15px 30px; margin: 10px; border-radius: 8px; cursor: pointer; }
    .arm { background-color: #f44336; color: white; border: none; }
    .disarm { background-color: #4CAF50; color: white; border: none; }
  </style>
</head>
<body>
  <h1>安防报警主机</h1>
  <p>当前状态: <strong id="status">未布防</strong></p >
  <button class="arm" onclick="sendCommand('arm')">布防 (Arm)</button>
  <button class="disarm" onclick="sendCommand('disarm')">撤防 (Disarm)</button>

  <script>
    function sendCommand(cmd) {
      fetch("/" + cmd).then(() => {
        document.getElementById("status").innerText = (cmd === 'arm') ? "已布防 (监控中)" : "未布防 (安全)";
      });
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nIP Address: " + WiFi.localIP().toString());

  // 路由配置
  server.on("/", []() { server.send(200, "text/html", htmlPage); });
  
  server.on("/arm", []() {
    isArmed = true;
    server.send(200, "text/plain", "Armed");
    Serial.println("系统已布防！");
  });

  server.on("/disarm", []() {
    isArmed = false;
    isAlarming = false;     // 解除锁定报警状态
    digitalWrite(ledPin, LOW); // 关灯
    server.send(200, "text/plain", "Disarmed");
    Serial.println("系统已撤防！");
  });

  server.begin();
}

void loop() {
  server.handleClient(); // 处理网络请求

  // 触发逻辑：如果在布防状态，且引脚被触摸，则锁定为报警状态
  if (isArmed && !isAlarming) {
    if (touchRead(touchPin) < threshold) {
      isAlarming = true; // 锁定报警
      Serial.println("警报触发！");
    }
  }

  // 报警执行逻辑：非阻塞式狂闪
  if (isAlarming) {
    if (millis() - lastBlinkTime > 50) { // 50ms 闪烁一次
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      lastBlinkTime = millis();
    }
  }
}