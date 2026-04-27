/*
 *  Thư viện cần cài:
 *  - WebSockets by Markus Sattler
 *
 *  Upload xong -> kết nối WiFi SUMO_0X
 *  -> mở 192.168.4.1 trên trình duyệt
 * ========================================
 */

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "web_interface.h"

// ========================================
//  ĐỔI ROBOT_ID CHO MỖI ROBOT (1 đến 7)
// ========================================
#define ROBOT_ID  1

// Mật khẩu WiFi (giống cho tất cả robot)
const char* WIFI_PASSWORD = "sumo2026";

// Kênh WiFi riêng biệt cho từng robot
const int WIFI_CHANNELS[] = {0, 1, 3, 5, 7, 9, 11, 13};

// ========================================
//  CHÂN KẾT NỐI ESP32 -> L298N
// ========================================
//
//  L298N #1 (DI CHUYỂN): ENA=25, IN1=26, IN2=27, ENB=14, IN3=13, IN4=5
//  (Đấu song song 2 motor TRÁI vào OUT1/OUT2, 2 motor PHẢI vào OUT3/OUT4)
//
//  L298N #2 (VŨ KHÍ): ENA=32, IN1=33, IN2=23, ENB=19, IN3=18, IN4=4
//  (Vũ khí 1 vào OUT1/OUT2, Vũ khí 2 vào OUT3/OUT4)

// L298N #1 - DI CHUYỂN
#define DRIVE_L_EN  25
#define DRIVE_L_IN1 26
#define DRIVE_L_IN2 27
#define DRIVE_R_EN  14
#define DRIVE_R_IN3 13
#define DRIVE_R_IN4  5

// L298N #2 - VŨ KHÍ
#define WEAPON1_EN  32
#define WEAPON1_IN1 33
#define WEAPON1_IN2 23
#define WEAPON2_EN  19
#define WEAPON2_IN3 18
#define WEAPON2_IN4  4

// PWM
#define PWM_FREQ 5000
#define PWM_RES  8

// LEDC channels
#define CH_DRIVE_L 0
#define CH_DRIVE_R 1
#define CH_WEAPON1 2
#define CH_WEAPON2 3

// ========================================
//  BIẾN TOÀN CỤC
// ========================================
WebServer server(80);
WebSocketsServer webSocket(81);

float joyX = 0, joyY = 0;
bool ramMode   = false;   // △ Lướt tới
bool spinLeft  = false;   // □ Xoay trái
bool spinRight = false;   // ○ Xoay phải
bool boostMode = false;   // R2 Turbo
bool weapon1Active = false; // L1 Vũ khí 1
bool weapon2Active = false; // R1 Vũ khí 2

unsigned long lastCmdTime = 0;
#define CMD_TIMEOUT 500

// Tốc độ preset
#define RAM_SPEED    255
#define SPIN_SPEED   200
#define NORMAL_MULT  0.75  // Tốc độ bình thường = 75%
#define BOOST_MULT   1.00  // Turbo R2 = 100%

// Client ID đang kết nối
uint8_t activeClient = 255; // 255 = không có ai

// ========================================
//  MOTOR FUNCTIONS
// ========================================

void setupMotors() {
  int pins[] = {DRIVE_L_IN1, DRIVE_L_IN2, DRIVE_R_IN3, DRIVE_R_IN4,
                WEAPON1_IN1, WEAPON1_IN2, WEAPON2_IN3, WEAPON2_IN4};
  for (int i = 0; i < 8; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }
  ledcSetup(CH_DRIVE_L, PWM_FREQ, PWM_RES); ledcAttachPin(DRIVE_L_EN, CH_DRIVE_L);
  ledcSetup(CH_DRIVE_R, PWM_FREQ, PWM_RES); ledcAttachPin(DRIVE_R_EN, CH_DRIVE_R);
  ledcSetup(CH_WEAPON1, PWM_FREQ, PWM_RES); ledcAttachPin(WEAPON1_EN, CH_WEAPON1);
  ledcSetup(CH_WEAPON2, PWM_FREQ, PWM_RES); ledcAttachPin(WEAPON2_EN, CH_WEAPON2);
}

void setLeftMotors(int speed) {
  bool fwd = (speed >= 0);
  int pwm = constrain(abs(speed), 0, 255);
  digitalWrite(DRIVE_L_IN1, fwd); digitalWrite(DRIVE_L_IN2, !fwd);
  ledcWrite(CH_DRIVE_L, pwm);
}

void setRightMotors(int speed) {
  bool fwd = (speed >= 0);
  int pwm = constrain(abs(speed), 0, 255);
  digitalWrite(DRIVE_R_IN3, fwd); digitalWrite(DRIVE_R_IN4, !fwd);
  ledcWrite(CH_DRIVE_R, pwm);
}

void setWeapon1(int speed) {
  bool fwd = (speed >= 0);
  int pwm = constrain(abs(speed), 0, 255);
  digitalWrite(WEAPON1_IN1, fwd); digitalWrite(WEAPON1_IN2, !fwd);
  ledcWrite(CH_WEAPON1, pwm);
}

void setWeapon2(int speed) {
  bool fwd = (speed >= 0);
  int pwm = constrain(abs(speed), 0, 255);
  digitalWrite(WEAPON2_IN3, fwd); digitalWrite(WEAPON2_IN4, !fwd);
  ledcWrite(CH_WEAPON2, pwm);
}

void stopAll() {
  ledcWrite(CH_DRIVE_L, 0); ledcWrite(CH_DRIVE_R, 0);
  ledcWrite(CH_WEAPON1, 0); ledcWrite(CH_WEAPON2, 0);
  digitalWrite(DRIVE_L_IN1, LOW); digitalWrite(DRIVE_L_IN2, LOW);
  digitalWrite(DRIVE_R_IN3, LOW); digitalWrite(DRIVE_R_IN4, LOW);
  digitalWrite(WEAPON1_IN1, LOW); digitalWrite(WEAPON1_IN2, LOW);
  digitalWrite(WEAPON2_IN3, LOW); digitalWrite(WEAPON2_IN4, LOW);
}

// Arcade drive: joystick (x,y) -> motor trái/phải
void arcadeDrive(float x, float y) {
  float left  = constrain(y + x, -1.0, 1.0);
  float right = constrain(y - x, -1.0, 1.0);
  setLeftMotors((int)(left * 255));
  setRightMotors((int)(right * 255));
}

// ========================================
//  XỬ LÝ LỆNH TỪ ĐIỆN THOẠI
// ========================================

// Gửi trạng thái motor thực tế về web để hiển thị speed bars
void sendMotorState(int leftPWM, int rightPWM) {
  if (activeClient != 255) {
    // Format: "M:left,right" (giá trị -255 đến 255)
    String state = "M:" + String(leftPWM) + "," + String(rightPWM);
    webSocket.sendTXT(activeClient, state);
  }
}

void processCommand() {
  int leftPWM = 0, rightPWM = 0;

  // Ưu tiên: nút bấm > joystick
  if (ramMode) {
    // △ Lướt tới: full speed tiến thẳng
    leftPWM = RAM_SPEED;
    rightPWM = RAM_SPEED;
  }
  else if (spinLeft) {
    // □ Xoay trái: trái lùi, phải tiến
    leftPWM = -SPIN_SPEED;
    rightPWM = SPIN_SPEED;
  }
  else if (spinRight) {
    // ○ Xoay phải: trái tiến, phải lùi
    leftPWM = SPIN_SPEED;
    rightPWM = -SPIN_SPEED;
  }
  else {
    // Joystick + boost multiplier
    float mult = boostMode ? BOOST_MULT : NORMAL_MULT;
    float left  = constrain((joyY + joyX) * mult, -1.0, 1.0);
    float right = constrain((joyY - joyX) * mult, -1.0, 1.0);
    leftPWM  = (int)(left * 255);
    rightPWM = (int)(right * 255);
  }

  setLeftMotors(leftPWM);
  setRightMotors(rightPWM);
  
  if (weapon1Active) setWeapon1(255);
  else setWeapon1(0);

  if (weapon2Active) setWeapon2(255);
  else setWeapon2(0);

  sendMotorState(leftPWM, rightPWM);
}

// ========================================
//  WEBSOCKET EVENTS
// ========================================

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WS] Client #%u ngắt\n", num);
      stopAll();
      ramMode = spinLeft = spinRight = boostMode = false;
      weapon1Active = weapon2Active = false;
      joyX = joyY = 0;
      activeClient = 255;
      break;

    case WStype_CONNECTED:
      Serial.printf("[WS] Client #%u kết nối\n", num);
      activeClient = num;
      break;

    case WStype_TEXT: {
      String msg = String((char*)payload);
      lastCmdTime = millis();

      // INFO: gửi robot ID
      if (msg == "INFO") {
        webSocket.sendTXT(num, "ID:" + String(ROBOT_ID));
        return;
      }

      // STOP: dừng khẩn cấp
      if (msg == "STOP") {
        stopAll();
        joyX = joyY = 0;
        ramMode = spinLeft = spinRight = boostMode = false;
        weapon1Active = weapon2Active = false;
        sendMotorState(0, 0);
        return;
      }

      // Joystick: "J:0.50,-0.80" (giá trị raw, chưa nhân boost)
      if (msg.startsWith("J:")) {
        String data = msg.substring(2);
        int comma = data.indexOf(',');
        if (comma > 0) {
          joyX = constrain(data.substring(0, comma).toFloat(), -1.0, 1.0);
          joyY = constrain(data.substring(comma + 1).toFloat(), -1.0, 1.0);
        }
        processCommand();
        return;
      }

      // Buttons: "B:TRI:1" hoặc "B:TRI:0"
      if (msg.startsWith("B:")) {
        String btn = msg.substring(2, msg.lastIndexOf(':'));
        bool pressed = msg.endsWith(":1");

        if (btn == "TRI") ramMode = pressed;         // △ Lướt
        else if (btn == "SQR") spinLeft = pressed;   // □ Xoay trái
        else if (btn == "CIR") spinRight = pressed;  // ○ Xoay phải
        else if (btn == "R2")  boostMode = pressed;  // R2 Turbo
        else if (btn == "L1")  weapon1Active = pressed; // L1 Vũ khí 1
        else if (btn == "R1")  weapon2Active = pressed; // R1 Vũ khí 2
        else if (btn == "CRS" && pressed) {          // ✕ Dừng
          stopAll();
          joyX = joyY = 0;
          ramMode = spinLeft = spinRight = boostMode = false;
          weapon1Active = weapon2Active = false;
          sendMotorState(0, 0);
          return;
        }
        // L2, R2 (nửa nút trên/dưới): dự phòng khác.

        processCommand();
        return;
      }
      break;
    }
    default: break;
  }
}

// ========================================
//  SETUP & LOOP
// ========================================

void setup() {
  Serial.begin(115200);
  delay(500);

  // 1. Motor
  setupMotors();
  stopAll();

  // 2. WiFi AP
  String ssid = "SUMO_0" + String(ROBOT_ID);
  int channel = WIFI_CHANNELS[ROBOT_ID];
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid.c_str(), WIFI_PASSWORD, channel, 0, 1);
  WiFi.setTxPower(WIFI_POWER_15dBm);

  Serial.println("\n============================");
  Serial.println("  ROBOT SUMO #" + String(ROBOT_ID));
  Serial.println("  WiFi: " + ssid);
  Serial.println("  Pass: " + String(WIFI_PASSWORD));
  Serial.println("  IP:   " + WiFi.softAPIP().toString());
  Serial.println("  Ch:   " + String(channel));
  Serial.println("============================\n");

  // 3. WebSocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // 4. HTTP Server
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", INDEX_HTML);
  });
  server.begin();

  Serial.println(">> Sẵn sàng! Kết nối WiFi -> mở 192.168.4.1 <<\n");
}

void loop() {
  webSocket.loop();
  server.handleClient();

  // Safety timeout
  if (millis() - lastCmdTime > CMD_TIMEOUT && lastCmdTime > 0) {
    if (joyX != 0 || joyY != 0 || ramMode || spinLeft || spinRight || boostMode || weapon1Active || weapon2Active) {
      stopAll();
      joyX = joyY = 0;
      ramMode = spinLeft = spinRight = boostMode = false;
      weapon1Active = weapon2Active = false;
      sendMotorState(0, 0);
      Serial.println("[SAFETY] Timeout - dừng motor");
    }
  }
}
