#include "message.h"

std::string build_message()
{
    // 保持消息独立成函数，方便演示静态库被主程序链接和调用。
    return "Hello from a CMake subdirectory project built with MinGW.";
}
