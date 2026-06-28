#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "时崎";
const char* password = "000000000";
const int LED_PIN = 2;  

// PWM配置
const int PWM_FREQ = 5000;
const int PWM_RES = 8;  // 0~255

WebServer server(80);

// 生成网页，内置滑动条+JS
String makePage(int currentPwm) {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>LED PWM滑动调光</title>
  <style>
    body {font-family:Arial; text-align:center; margin-top:50px;}
    #slider {width: 80%; max-width: 400px; height: 10px;}
    .val {font-size:22px; color:#d32f2f; font-weight:bold;}
  </style>
</head>
<body>
  <h1>PWM滑动条控制LED亮度</h1>
  <p>当前亮度值：<span class="val" id="showVal">)rawliteral" + String(currentPwm) + R"rawliteral(</span></p>
  <!-- 滑动条 0~255 -->
  <input type="range" id="pwmSlider" min="0" max="255" value=")rawliteral" + String(currentPwm) + R"rawliteral(" step="1">

  <script>
    const slider = document.getElementById('pwmSlider');
    const showVal = document.getElementById('showVal');

    // 滑动条变动触发
    slider.addEventListener('input', function(){
      let val = this.value;
      showVal.innerText = val;
      // 异步GET请求发送亮度数值
      fetch(`/set?pwm=${val}`)
        .catch(err => console.log("发送失败:", err));
    });
  </script>
</body>
</html>
)rawliteral";
  return html;
}

// 根页面，读取当前PWM值传入网页
void handleRoot() {
  int nowDuty = ledcRead(LED_PIN);
  server.send(200, "text/html; charset=UTF-8", makePage(nowDuty));
}

// 处理滑动条发来的亮度接口 /set?pwm=数字
void handleSetPWM() {
  int pwmVal = 0;
  // 判断URL是否携带pwm参数
  if (server.hasArg("pwm")) {
    pwmVal = server.arg("pwm").toInt();
    // 限制范围0~255，防止越界
    if (pwmVal < 0) pwmVal = 0;
    if (pwmVal > 255) pwmVal = 255;
    ledcWrite(LED_PIN, pwmVal);
  }
  // 无需跳转，返回空响应
  server.send(200, "text/plain", "ok");
}

void setup() {
  Serial.begin(115200);
  
  // 新版一次性绑定引脚、频率、分辨率
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RES);
  ledcWrite(LED_PIN, 0); // 初始熄灭

  // WiFi连接
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功");
  Serial.print("网页地址：http://");
  Serial.println(WiFi.localIP());

  // 绑定路由
  server.on("/", handleRoot);
  server.on("/set", handleSetPWM);
  server.begin();
}

void loop() {
  server.handleClient();
}