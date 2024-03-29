#include <string.h> // 包含 memcmp 函数所需的头文件
#include <stdbool.h> // 包含 bool 类型所需的头文件
#include "utils.h"
// 比较两个字符串的前部分字符及其长度
// 返回值:
//   true: str1 == str2
//   false: str1 != str2
bool stringCompare(const char *str1, size_t len1, const char *str2, size_t len2) {
    // 首先比较长度
    if (len1 != len2) {
        return false;
    }

    // 比较两个字符串的前 len1 和 len2 个字符
    return memcmp(str1, str2, len1) == 0;
}