// 定义LED引脚
const int ledPin_r = 5;  // 红色LED
const int ledPin_g = 4;  // 绿色LED

// PWM参数
const int freq = 5000;
const int resolution = 8;
// 步进值，越小过渡越顺滑，循环越慢
const float step = 0.02f;
float angle = 0.0f;

void setup() {
  Serial.begin(115200);
  ledcAttach(ledPin_g, freq, resolution);
  ledcAttach(ledPin_r, freq, resolution);
}

void loop() {
  // sin范围 [-1, 1]，映射到亮度区间 0~255
  // 红灯：(sinθ + 1) / 2 → 0~255
  // 绿灯：(sin(θ+π) + 1) / 2 = (-sinθ + 1) / 2，与红灯完全反向
  int redDuty  = (sin(angle) + 1.0f) * 127.5f;
  int greenDuty = (-sin(angle) + 1.0f) * 127.5f;

  ledcWrite(ledPin_r, redDuty);
  ledcWrite(ledPin_g, greenDuty);

  angle += step;
  // 角度溢出重置，防止浮点过大
  if (angle > 2 * PI) {
    angle = 0.0f;
    Serial.println("完整呼吸周期完成");
  }

  delay(8);
}