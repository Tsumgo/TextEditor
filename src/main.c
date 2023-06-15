#include <windows.h>
#include <winuser.h>
#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"

#include "keyboardsupport.h"
#include "timersupport.h"
#include "mousesupport.h"
#include "charsupport.h"
#include "display.h"
#include "model.h"
#include "edit.h"
#include "file.h"

#define REFREASH 1

// 全局变量
static double winwidth, winheight; // 窗口尺寸

// 仅初始化执行一次
void Main()
{
    // 初始化窗口和图形系统
    SetWindowTitle("TextEditor");
    // SetWindowSize(10, 10); // 单位 - 英寸
    // SetWindowSize(15, 10);
    // SetWindowSize(10, 20); // 如果屏幕尺寸不够，则按比例缩小
    InitGraphics();

    // 获得窗口尺寸
    winwidth = GetWindowWidth();
    winheight = GetWindowHeight();
    initDisplay();
    initEdit();
    initFileConfig();

    initModel();
    // 注册时间响应函数
    registerCharEvent(CharEventProcess);         // 字符
    registerKeyboardEvent(KeyboardEventProcess); // 键盘
    registerMouseEvent(MouseEventProcess);       // 鼠标
    registerTimerEvent(TimerEventProcess);       // 定时器

    puts("Event registered!");
    startTimer(REFREASH, 500);

    // 打开控制台，方便用printf/scanf输出/入变量信息，方便调试
    InitConsole();
}