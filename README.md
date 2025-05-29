# Dự án DS1307 - Hiển thị thời gian thực trên LCD

Dự án sử dụng vi điều khiển AVR để đọc dữ liệu thời gian thực từ module DS1307 thông qua giao tiếp I2C và hiển thị lên LCD 16x2.

## Mục tiêu

- Đọc thời gian thực từ DS1307.
- Hiển thị ngày, giờ lên LCD.
- Mô phỏng bằng Proteus.
- Lập trình bằng Microchip Studio.
- Nạp chương trình bằng ProgISP.

## Phần cứng sử dụng
- Kít phát triển AVR. 
- Vi điều khiển: ATmega16.
- Module RTC: DS1307 + pin dự phòng.
- LCD 16x2 điều khiển qua giao tiếp I2C.
- Dây nối, điện trở, thạch anh, tụ lọc, nút nhấn (tùy chọn).

## Phần mềm

- Microchip Studio (lập trình C, biên dịch ra file .hex).
- Proteus (mô phỏng mạch).
- ProgISP (nạp file .hex vào vi điều khiển AVR).

## Cấu trúc thư mục
```
NHT_DoAnII_DTVT_K67_2025/
├── Image/                 # Hình ảnh liên quan đến đồ án (mạch mô phỏng, kết quả, sơ đồ, v.v.)
├── Project2_NHT/          # Thư mục chứa file mô phỏng Proteus (*.pdsprj, *.dsn)
├── SENSOR_DS1307/         # Mã nguồn C, dùng trong Microchip Studio
│   ├── src/               # File mã nguồn (*.c, *.h)
│   ├── Debug/             # File build (tự sinh bởi IDE, được bỏ qua trong Git)
│   └── Makefile hoặc *.atsln  # File dự án 
├── BaoCao_DoAn.pdf        # File báo cáo đồ án
├── .gitignore             # Tệp cấu hình bỏ qua file/thư mục khi dùng Git
└── README.md              # Tệp mô tả dự án (file này)
```


## Hướng dẫn sử dụng

1. **Lập trình và biên dịch mã nguồn**
   - Truy cập `SENSOR_DS1307/`.
   - Mở project trong Microchip Studio.
   - Chỉnh sửa và biên dịch để tạo file `.hex`.

2. **Mô phỏng mạch**
   - Mở `Project2_NHT/` bằng Proteus.
   - Gắn file `.hex` vào vi điều khiển trong sơ đồ mô phỏng.
   - Chạy mô phỏng để kiểm tra kết quả hiển thị.

3. **Nạp chương trình thực tế**
   - Mở phần mềm ProgISP.
   - Chọn điều khiển ATmega16 và nạp file `.hex` vào mạch thực.

4. **Xem tài liệu và kết quả**
   - Xem báo cáo trong file `BaoCao_DoAn.pdf`.
   - Tham khảo hình ảnh kết quả hoặc sơ đồ mô phỏng trong thư mục `"Image\duonglich (2).jpg"`.
   - 
  ![Lịch âm](https://raw.githubusercontent.com/eouinht/NHT_DoAnII_DTVT_K67_2025/main/Image/amlich%20(2).jpg)


## Giấy phép

Dự án này được thực hiện phục vụ mục đích học tập và nghiên cứu tại trường Đại học. Người dùng có thể tham khảo, chỉnh sửa hoặc sử dụng lại cho mục đích cá nhân hoặc học thuật, nhưng vui lòng không sử dụng cho mục đích thương mại và cần ghi rõ nguồn gốc nếu chia sẻ lại.
