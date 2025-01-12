ChatApplication
    - show các command 
    - xử lý command
    - gọi ra connection manager
ConnectionManager
    - Quản lý kết nối
        - Thêm 1 kết nối
        - Sửa 1 kết nối
        - Xóa 1 kết nối
        - Healcheck/HeartBeat/KeepAlive
        - hiển thị các kết nối đang hoạt động
        - hiển thị ip/port

TCPSocket
    - Mở/tạo một socket
    - Gửi dữ liệu
    - nhận dữ liệu
    - Get ip/port của client

Lưu ý:
    - Cần tạo ra một mảng fd[] để lưu trữ thông tin của socket 
    - Khi chạy chương trình mọi người có một socket (socket server) - fd[0]
    - Khi có một client kết nối tơi thì sẽ làm việc với socket server
    - Sau khi client kết nối thành công, sẽ tạo một client1 socket mới đẻ trao đổi dữ liệu - fd[1]
    - Sau khi client kết nối thành công, sẽ tạo một clientn socket mới đẻ trao đổi dữ liệu - fd[n]
    - Sử dụng poll(), bất cứ khi nào có dữ liệu được ghi vào fd[0-n] thì poll() sẽ xác định được đâu là client đang muốn giao tiếp.

Yêu cầu:
    - Thời gian: 1 tuần
    - Tuân thủ conding convention
    - Code không được xảy ra memleak, segment fault (valgrind)
    - Sử dụng Makefile hoặc Cmake
    - Ngôn ngữ: c or cpp
