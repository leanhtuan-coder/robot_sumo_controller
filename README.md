# 🤖 Robot Sumo Battle Controller

> Hệ thống điều khiển robot sumo 4WD qua WiFi bằng joystick ảo trên điện thoại.
> Thiết kế cho cuộc thi **Lê Lợi Robotics Challenge 2026** — 7 đội, thi đấu 1v1.
>
> *Designed by Anh Tuan Le | VEX Technology Solutions*

---

## 📦 Linh kiện mỗi robot

| Linh kiện | Số lượng | Chức năng |
|-----------|----------|-----------|
| ESP32 DevKit v1 (CP2102) | 1 | Bộ não điều khiển + WiFi |
| Module L298N | 2 | Điều khiển 4 motor DC |
| Động cơ giảm tốc vàng 3-9V + bánh xe | 4 | Di chuyển |
| Đế pin 18650 (2 pin nối tiếp) | 1 | Nguồn 7.4V |
| Pin 18650 | 2 | Nguồn năng lượng |
| Dây nối đực-cái | ~20 sợi | Kết nối |

---

## 🔌 Sơ đồ đấu nối

### ESP32 → L298N #1 (Bên TRÁI)

```
ESP32 Pin    →    L298N #1 Pin    →    Motor
─────────────────────────────────────────────
GPIO 25      →    ENA             →    Tốc độ Trước-Trái (PWM)
GPIO 26      →    IN1             →    Chiều quay Trước-Trái
GPIO 27      →    IN2             →    Chiều quay Trước-Trái
GPIO 14      →    ENB             →    Tốc độ Sau-Trái (PWM)
GPIO 13      →    IN3             →    Chiều quay Sau-Trái
GPIO 5       →    IN4             →    Chiều quay Sau-Trái
```

### ESP32 → L298N #2 (Bên PHẢI)

```
ESP32 Pin    →    L298N #2 Pin    →    Motor
─────────────────────────────────────────────
GPIO 32      →    ENA             →    Tốc độ Trước-Phải (PWM)
GPIO 33      →    IN1             →    Chiều quay Trước-Phải
GPIO 23      →    IN2             →    Chiều quay Trước-Phải
GPIO 19      →    ENB             →    Tốc độ Sau-Phải (PWM)
GPIO 18      →    IN3             →    Chiều quay Sau-Phải
GPIO 4       →    IN4             →    Chiều quay Sau-Phải
```

### Nguồn điện

```
Pin 18650 (2 viên nối tiếp = 7.4V)
    │
    ├── (+) ──→ L298N #1 VCC (12V input)
    ├── (+) ──→ L298N #2 VCC (12V input)
    │
    ├── (–) ──→ L298N #1 GND
    ├── (–) ──→ L298N #2 GND
    └── (–) ──→ ESP32 GND

L298N #1 (5V output) ──→ ESP32 VIN (cấp nguồn cho ESP32)
```

> ⚠️ **LƯU Ý**: Nhớ nối GND chung giữa ESP32 và cả 2 module L298N!

### Sơ đồ tổng quan

```
                    ┌─────────────┐
                    │    ESP32    │
                    │   DevKit   │
                    └──┬──┬──┬──┬┘
                       │  │  │  │
            ┌──────────┘  │  │  └──────────┐
            │             │  │             │
     ┌──────┴──────┐      │  │      ┌──────┴──────┐
     │  L298N #1   │      │  │      │  L298N #2   │
     │  (TRÁI)     │      │  │      │  (PHẢI)     │
     └──┬──────┬───┘      │  │      └──┬──────┬───┘
        │      │           │  │         │      │
   ┌────┘  ┌───┘           │  │    ┌────┘  ┌───┘
   │       │               │  │    │       │
 [M1]    [M2]              │  │  [M3]    [M4]
 Trước   Sau               │  │  Trước   Sau
 Trái    Trái              │  │  Phải    Phải
                           │  │
                    ┌──────┴──┴──────┐
                    │  Pin 18650 x2  │
                    │    (7.4V)      │
                    └────────────────┘
```

---

## 💻 Cài đặt phần mềm

### 1. Cài Arduino IDE
- Tải từ: https://www.arduino.cc/en/software

### 2. Thêm ESP32 Board
1. Mở Arduino IDE → **File** → **Preferences**
2. Trong **Additional Board Manager URLs**, thêm:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Vào **Tools** → **Board** → **Boards Manager**
4. Tìm **esp32** và cài đặt (bởi Espressif Systems)

### 3. Cài thư viện WebSocket
1. Vào **Sketch** → **Include Library** → **Manage Libraries**
2. Tìm **WebSockets** bởi **Markus Sattler**
3. Nhấn **Install**

### 4. Cấu hình Board
- **Board**: ESP32 Dev Module
- **Upload Speed**: 921600
- **CPU Frequency**: 240MHz
- **Flash Size**: 4MB
- **Port**: Chọn cổng COM tương ứng

---

## 🚀 Cách sử dụng

### Bước 1: Cấu hình Robot ID
Mở file `robot_sumo_controller.ino`, đổi dòng:
```cpp
#define ROBOT_ID  1  // Đổi thành 1, 2, 3, 4, 5, 6, hoặc 7
```

### Bước 2: Upload code
- Kết nối ESP32 qua USB
- Nhấn **Upload** trong Arduino IDE
- Đợi upload xong, mở **Serial Monitor** (115200 baud) để kiểm tra

### Bước 3: Kết nối điện thoại
1. Mở **Cài đặt WiFi** trên điện thoại
2. Tìm và kết nối vào mạng **SUMO_01** (hoặc SUMO_02, ...)
3. Nhập mật khẩu: **sumo2026**
4. Mở **trình duyệt** (Chrome/Safari)
5. Truy cập: **192.168.4.1**

### Bước 4: Điều khiển
- **Kéo joystick** để di chuyển robot
- **Lên/Xuống** = Tiến/Lùi
- **Trái/Phải** = Rẽ trái/phải
- **Nhấn STOP** = Dừng khẩn cấp

---

## 📡 Chiến lược chống nhiễu WiFi (7 robot cùng lúc)

| Robot | SSID | Kênh WiFi | Khoảng cách kênh |
|-------|------|-----------|-------------------|
| 1 | SUMO_01 | 1 | — |
| 2 | SUMO_02 | 3 | +2 |
| 3 | SUMO_03 | 5 | +2 |
| 4 | SUMO_04 | 7 | +2 |
| 5 | SUMO_05 | 9 | +2 |
| 6 | SUMO_06 | 11 | +2 |
| 7 | SUMO_07 | 13 | +2 |

**Các biện pháp chống nhiễu:**
- ✅ Mỗi robot dùng kênh WiFi riêng, cách nhau 2 kênh
- ✅ Giảm công suất phát WiFi (15dBm thay vì 20dBm)
- ✅ Giới hạn 1 client kết nối mỗi robot
- ✅ WebSocket gửi data nhỏ (~10 bytes/message)
- ✅ Tần suất gửi 25Hz (đủ mượt, không quá tải)

---

## 🔧 Xử lý sự cố

| Vấn đề | Giải pháp |
|--------|-----------|
| Không thấy WiFi SUMO_0X | Kiểm tra ESP32 có đang chạy (LED xanh nhấp nháy). Reset ESP32. |
| Kết nối WiFi nhưng không mở được trang | Đảm bảo nhập đúng **192.168.4.1**. Tắt mobile data. |
| Joystick lag/giật | Đứng gần robot hơn. Kiểm tra xem có thiết bị WiFi khác gây nhiễu. |
| Motor quay sai chiều | Đổi 2 dây motor hoặc đổi IN1/IN2 trong code. |
| Motor không quay | Kiểm tra kết nối dây, nguồn pin, jumper 5V trên L298N. |
| Robot chạy lệch | Điều chỉnh cơ khí hoặc thêm hệ số bù trong code. |

---

## 📁 Cấu trúc dự án

```
robot-sumo-battle/
├── robot_sumo_controller/
│   └── robot_sumo_controller.ino   ← Code chính (upload lên ESP32)
├── test_controller.html             ← Test joystick trên máy tính
└── README.md                        ← File này
```

---

## 🏆 Tips thi đấu

1. **Sạc đầy pin** trước mỗi trận
2. **Test kết nối WiFi** trước khi vào sân
3. **Giữ điện thoại gần robot** (< 10m) để giảm lag
4. **Tắt mobile data** trên điện thoại khi điều khiển
5. **Dùng trình duyệt Chrome** để tương thích tốt nhất
