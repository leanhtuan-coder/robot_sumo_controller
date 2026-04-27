# Changelog

Tất cả thay đổi đáng chú ý của dự án được ghi lại tại đây.

## [1.0.0] — 2026-04-27

### Phiên bản đầu tiên

- Firmware ESP32 điều khiển 4 motor DC qua 2 module L298N
- Giao diện web kiểu tay cầm PS2 (joystick + nút hành động)
- Kết nối WiFi AP mode — mỗi robot 1 mạng riêng
- Chống nhiễu WiFi cho 7 robot hoạt động đồng thời (7 kênh riêng biệt)
- Giao thức WebSocket cho điều khiển real-time (25Hz)
- Nút hành động: Lướt (△), Xoay trái/phải (□/○), Dừng khẩn cấp (✕)
- Chế độ Turbo (R2) tăng tốc độ từ 75% lên 100%
- Safety timeout — tự dừng motor khi mất kết nối
- ESP32 gửi trạng thái motor thực tế về web hiển thị trên speed bars
