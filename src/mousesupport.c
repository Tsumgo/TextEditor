#include <windows.h>
#include <winuser.h>
#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"

#include "mousesupport.h"
#include "model.h"
#include "display.h"

extern double textBoxHeight;
extern double textBoxWidth;
extern double menuHeight;
extern double sideBarWidth;
static PtrToLine curLine;

static int winHeight;
static int winWidth;
static blockNode winCurrent;

static int isButtonDown = 0;

/// @brief this fucntion get the index of the current char
/// @param x the x cord in inch
/// @param buffer the head pointer of the line
/// @return the index of the selected position
static int locateX(double x, int len, char *buffer);

static int locateX(double x, int len, char *buffer)
{
    int nowx;
    char *start = buffer; // the start of the line
    if (x <= sideBarWidth)
        nowx = 0;
    else
        nowx = (x - sideBarWidth) / textBoxWidth;
    buffer += nowx;

    // Dealing with the Cursor Problem of Chinese Characters
    char *temp = buffer;
    while ((*temp & 0x80) && temp >= start)
        temp--;              // back to the start of the chinese chars
    int min = buffer - temp; // the length of the chinese chars
    if (min % 2 == 0 && min != 0)
        nowx++; // if in middle of the char, add 1

    if ((len - 1) < nowx)
        nowx = len - 1;
    return nowx;
}

static int locateY(double y)
{
    int nowy = winCurrent.row + (int)((winHeight - menuHeight / 2 - y) / (textBoxHeight)) - 1;
    if (getTotalRow() - 1 < nowy)
        nowy = getTotalRow() - 1;
    if (nowy < 0)
        nowy = 0;
    return nowy;
}

// Return the row and column number of the current mouse position
blockNode getLocation(double x, double y)
{
    int newRow, newCol;
    double nx = ScaleXInches(x);
    double ny = ScaleYInches(y);
    newRow = locateY(ny);
    gotoXLine(newRow);
    newCol = locateX(nx, getCurLine()->Len, getCurLine()->Text);
    return (blockNode){newRow, newCol};
}

void MouseEventProcess(int x, int y, int button, int event)
{
    uiGetMouse(x, y, button, event);
    winHeight = GetWindowHeight();
    winWidth = GetWindowWidth();
    winCurrent = getWindowCurrent();

    switch (event)
    {
    case BUTTON_DOWN:
        if (button == LEFT_BUTTON)
        {
            puts("button down");
            blockNode mouse = getLocation(x, y);
            isButtonDown = 1;

            setCursor(mouse);
            setStartSelect(mouse);
            setEndSelect(mouse);
            // setCursorInWindow();
        }
        // if (getContextMenuDisplayState())
        //     setContextMenuDisplayState(0); // 不显示右键菜单
        break;
    case MOUSEMOVE:
        if (isButtonDown)
        { // 鼠标拖动选择
            blockNode mouse = getLocation(x, y);
            setCursor(mouse);
            setEndSelect(mouse);
            // setCursorInWindow();
        }
        break;
    case BUTTON_UP:
        if (button == LEFT_BUTTON)
        {
            blockNode mouse = getLocation(x, y);
            isButtonDown = 0;
            setCursor(mouse);
            setEndSelect(mouse);
            // setCursorInWindow();
        }
        break;
    }
    display(); // 刷新显示
}
