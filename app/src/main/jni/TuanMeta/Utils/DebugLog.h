#pragma once
#include <string>
#include <vector>

// Khai báo hàm log
void AddDebugLog(const char* fmt, ...);

// Xóa tất cả log
void ClearDebugLogs();

// Trả về danh sách log hiện tại (read-only)
const std::vector<std::string>& GetDebugLogs();
