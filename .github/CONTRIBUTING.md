# Hướng dẫn đóng góp (Contributing Guidelines)

Cảm ơn bạn đã quan tâm đến việc đóng góp cho dự án **Robot Sumo Battle Controller** của **VEX Technology Solutions**! Chúng tôi hoan nghênh mọi đóng góp từ cộng đồng học sinh, giáo viên và các nhà phát triển.

## Làm thế nào để đóng góp?

### 1. Báo cáo lỗi (Report Bugs)
Nếu bạn phát hiện lỗi trong quá trình sử dụng bộ điều khiển (kết nối rớt, giao diện lỗi, motor chạy không đúng), vui lòng tạo một Issue mới. Hãy sử dụng mẫu (template) **Bug Report** và cung cấp đầy đủ thông tin:
- Phiên bản Arduino IDE, phiên bản thư viện ESP32.
- Dòng điện thoại và trình duyệt bạn đang sử dụng.
- Mô tả chi tiết cách tái hiện lỗi.

### 2. Đề xuất tính năng (Feature Requests)
Bạn có ý tưởng hay cho chiến thuật điều khiển hoặc muốn nâng cấp giao diện? Hãy mở một Issue bằng mẫu **Feature Request** để chúng ta cùng thảo luận trước khi bắt tay vào code.

### 3. Đóng góp Code (Pull Requests)
1. Fork dự án (Nhấn nút **Fork** ở góc phải repository).
2. Clone bản fork về máy của bạn: `git clone https://github.com/<username_cua_ban>/robot_sumo_controller.git`
3. Tạo một branch mới cho tính năng hoặc bản sửa lỗi của bạn: `git checkout -b feature/ten-tinh-nang`
4. Viết code.
5. Commit thay đổi với thông điệp rõ ràng: `git commit -m "Thêm tính năng X"`
6. Push lên branch của bạn: `git push origin feature/ten-tinh-nang`
7. Mở một **Pull Request (PR)** từ branch của bạn vào branch `main` của repository gốc.

## Tiêu chuẩn Code (Coding Standards)
- **C/C++ (Arduino)**: Sử dụng indent 2 spaces. Đặt tên biến theo chuẩn `camelCase`. Thêm comment giải thích cho các đoạn logic phức tạp.
- **HTML/JS/CSS**: Giao diện được thu gọn trong file `web_interface.h`. Nếu bạn thay đổi giao diện, hãy cập nhật lại file này cẩn thận để tránh lỗi parse chuỗi `PROGMEM`.

## Câu hỏi & Hỗ trợ
Nếu bạn cần trợ giúp kỹ thuật liên quan đến giải đấu **Lê Lợi Robotics Challenge 2026**, vui lòng liên hệ:
- Email: contact.vextech@gmail.com
- Fanpage: [VEX Technology Solutions](https://www.facebook.com/vex.technology.solutions)
