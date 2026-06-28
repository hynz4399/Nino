// 定义触摸引脚 (T0对应GPIO4)
#define TOUCH_PIN 4
// 定义LED引脚 (ESP32 DevKit板载LED通常是GPIO2)
#define LED_PIN 2
// 中断模式设置：0为轮询模式，1为中断模式
#define EXT_ISR_MODE 0

// 阈值，需要通过串口监视器观察并调整
int threshold = 600; 
// 触摸值
int touchValue;
// LED状态标记
bool ledState = false;
// 触摸防抖标记，防止长按反复触发
bool isTouching = false;

void setup() {
  Serial.begin(115200);
  delay(1000); // 等待串口稳定

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // 初始LED熄灭
}

void loop() {
  touchValue = touchRead(TOUCH_PIN);
  Serial.print("Touch Value: ");
  Serial.println(touchValue);

  // 检测到触摸
  if (touchValue < threshold) {
    isTouching = true;
  } 
  // 松手瞬间，且之前处于触摸状态，切换LED
  else if (isTouching == true) {
    ledState = !ledState; // 翻转LED状态
    digitalWrite(LED_PIN, ledState);
    isTouching = false;   // 清除触摸标记，等待下一次触摸
    Serial.print("LED状态切换: ");
    Serial.println(ledState ? "亮" : "灭");
  }
  
  delay(100);
}