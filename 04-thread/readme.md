Việc sử dụng thread và tránh deadlock trong môi trường đa luồng
Các hàm pthread_create(), pthread_join()
Tạo mutex tĩnh , tạo mutexx động .Vào việc critical section . 
Sử dụng Condition Variable để thông báo cho các luồng khác và thực hiện chạy luồng nếu nhận được tín hiệu : pthread_cond_t(),pthread_cond_wait(), pthread_cond_signal(). 
Xây dựng bài toán producer và consumer để truyền tải và lấy dữ liệu một cách đồng bộ tránh xung đột . 
Trong trường hợp nhiều luồng (thread) cùng truy cập vào 1 tài nguyên (shared source) cần sử dụng mutex để tránh việc bị deadlock
Khi lock và unlock mutex trong nhiều luồng cần tuân theo thứ tự khóa và mở khóa mutex để tránh bị deadlock
