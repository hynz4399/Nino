// 硬件引脚定义
#define TOUCH_PIN 4    // T0触摸引脚GPIO4
#define LED_PIN 2      // 板载LED PWM引脚

// 触摸阈值，根据串口打印数值微调
int touchThreshold = 600;
int touchValue;

// 触摸状态防抖标记
bool isTouchHold = false;

// 呼吸灯档位与参数
int gear = 1;          // 当前档位：1/2/3
int ledBright = 0;     // LED当前亮度 0~255
int fadeStep;          // 亮度变化步长，控制呼吸快慢
bool fadeDir = true;   // 渐变方向：true变亮 / false变暗

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0); // 初始熄灭
}

void loop() {
  // 1. 读取触摸传感器
  touchValue = touchRead(TOUCH_PIN);
  Serial.print("Touch: ");
  Serial.print(touchValue);
  Serial.print(" | 档位: ");
  Serial.println(gear);

  // 触摸松手切换档位逻辑
  if (touchValue < touchThreshold) {
    isTouchHold = true;
  } else if (isTouchHold) {
    // 松手触发档位循环
    gear++;
    if (gear > 3) gear = 1;
    isTouchHold = false;
    Serial.print("切换至档位 ");
    Serial.println(gear);
  }

  // 2. 根据档位设置呼吸步长（步长越大呼吸越快）
  switch (gear) {
    case 1: fadeStep = 1; break; // 慢呼吸
    case 2: fadeStep = 3; break; // 中等速度
    case 3: fadeStep = 6; break; // 急促快呼吸
  }

  // 3. PWM呼吸渐变逻辑
  if (fadeDir) {
    ledBright += fadeStep;
  } else {
    ledBright -= fadeStep;
  }

  // 到达亮度边界时反转渐变方向
  if (ledBright >= 255) {
    ledBright = 255;
    fadeDir = false;
  }
  if (ledBright <= 0) {
    ledBright = 0;
    fadeDir = true;
  }

  analogWrite(LED_PIN, ledBright);
  delay(10); // 基础刷新延时，统一帧率，仅靠步长区分速度
}