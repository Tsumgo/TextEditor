#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <winuser.h>

#include "graphics.h"
#include "model.h"
#include "extgraph.h"
#include "imgui.h"
#include "strlib.h"
#include "genlib.h"
#include "edit.h"
#include "file.h"
#include "display.h"
#define max(x, y) (x) > (y) ? (x) : (y)

static int lockDisplay = 0;
static int isShowHelp = 0;        //   show help page
static int isShowAbout = 0;       //   show about page
static int isShowKeyboard = 0;    //   show keyboard help page
static double textPointSize = 15; //   这里调整字体大小

double textBoxHeight; // 文本框的高度
double textBoxWidth;  // 单个字符宽度
double menuHeight;    // 菜单栏的高度
double sideBarWidth;  // 侧边栏宽度

static double winHeight; // 屏幕高度
static double winWidth;  // 屏幕宽度

int maxShowR; // 当前界面最多展示的行数
int maxShowC; // 当前界面最多展示的列数

void CreateColors()
{
    DefineColor("menuFrame", 7.0 / 255, 143.0 / 255, 149.0 / 255);
    DefineColor("menuHotFrame", 0.0 / 255, 255.0 / 255, 255.0 / 255);
    DefineColor("menuLable", 255.0 / 255, 204.0 / 255, 204.0 / 255);
    DefineColor("menuhotLable", 255.0 / 255, 102.0 / 255, 102.0 / 255);
    DefineColor("Pink", 255.0 / 255, 192.0 / 255, 203.0 / 255);
    DefineColor("Background", 30.0 / 255, 30.0 / 255, 30.0 / 255);
    DefineColor("backgroundcolor", 255.0 / 255, 255.0 / 255, 204.0 / 255);
    DefineColor("Selected", 38.0 / 255, 79.0 / 255, 120.0 / 255);
    DefineColor("Purple", 204.0 / 255, 204.0 / 255, 255.0 / 255);
    DefineColor("Miku", 57.0 / 255, 197.0 / 255, 187.0 / 255);
    DefineColor("Tianyi", 102.0 / 255, 204.0 / 255, 255.0 / 255);
    DefineColor("Grey", 128.0 / 255, 128.0 / 255, 128.0 / 255);
    DefineColor("Sideline", 153.0 / 255, 204.0 / 255, 0.0 / 255);
}
void initDisplay()
{
    CreateColors();
    setMenuColors("menuFrame", "menuLable", "menuHotFrame", "menuhotLable", 1);
    setTextBoxColors("White", "BLUE", "White", "BLUE", 0);

    SetPointSize(textPointSize);
    SetFont("Consolas");
    double ASCIIH = GetFontHeight();
    double ASCIIW = TextStringWidth("W");
    SetFont("Microsoft YaHei UI");
    double ChH = GetFontHeight();
    double ChW = TextStringWidth("你");
    textBoxHeight = max(ASCIIH, ChH);
    textBoxWidth = max(ASCIIW, ChW / 2);

    winHeight = GetWindowHeight();
    winWidth = GetWindowWidth();

    sideBarWidth = 0.5;

    maxShowR = (winHeight - menuHeight) / textBoxHeight - 1; // 最多展示的行数
    maxShowC = (winWidth - sideBarWidth) / textBoxWidth - 1; // 最多展示的列数
}

static void drawMenu()
{
    SetFont("微软雅黑");
    SetPointSize(13);
    SetStyle(1);
    static char *menuListFile[] = {"File",
                                   "New File     Ctrl-N",
                                   "Open File    Ctrl-O",
                                   "Save         Ctrl-S",
                                   "Exit         Ctrl-E"};
    static char *menuListEdit[] = {"Edit",
                                   "Undo        Ctrl-Z",
                                   "Redo        Ctrl-Y",
                                   "Cut          Ctrl-X",
                                   "Copy        Ctrl-C",
                                   "Paste       Ctrl-V",
                                   "Find         Ctrl-F",
                                   "Replace    Ctrl-H"};
    static char *menuListHelp[] = {"Help",
                                   "Keyboard Shortcuts Reference",
                                   "About                       "};
    static char *selectedLabel = NULL;

    double fH = GetFontHeight();
    double x = 0;
    double y = winHeight;
    menuHeight = fH * 1.5;
    double w = TextStringWidth(menuListHelp[0]) * 2;
    double wlist = 0;
    int selection;
    drawMenuBar(0, y - menuHeight, winWidth, menuHeight);

    // File Menu
    wlist = TextStringWidth(menuListFile[1]) * 1.2;
    selection = menuList(GenUIID(0), x, y - menuHeight, w, wlist, menuHeight, menuListFile, sizeof(menuListFile) / sizeof(menuListFile[0]));
    if (selection >= 0)
        selectedLabel = menuListFile[selection];
    switch (selection)
    {
    case 0:
        break;
    case 1:
        newFile();
        break;
    case 2:
        openFile();
        break;
    case 3:
        saveFile();
        break;
    case 4:
        exitApplication();
        break;
    }

    // Edit menu
    wlist = TextStringWidth(menuListEdit[1]) * 1.2;
    selection = menuList(GenUIID(0), x + w, y - menuHeight, w, wlist, menuHeight, menuListEdit, sizeof(menuListEdit) / sizeof(menuListEdit[0]));
    if (selection >= 0)
        selectedLabel = menuListEdit[selection];
    switch (selection)
    {
    case 0:
        break;
    case 1:
        UnDo();
        break;
    case 2:
        ReDo();
        break;
    case 3:
        Cut();
        break;
    case 4:
        Copy();
        break;
    case 5:
        Paste();
        break;
    }

    // Help menu
    wlist = TextStringWidth(menuListHelp[1]) * 1.2;
    selection = menuList(GenUIID(0), x + 2 * w, y - menuHeight, w, wlist, menuHeight, menuListHelp, sizeof(menuListHelp) / sizeof(menuListHelp[0]));
    if (selection >= 0)
        selectedLabel = menuListHelp[selection];
    switch (selection)
    {
    case 0:
        break;
    case 1:
        isShowKeyboard = 1;
        break;
    case 2:
        isShowAbout = 1;
        break;
    }

    // 下面三行代码是历史遗留产物，删了怪可惜的，留着做纪念吧
    //  SetPenColor("Blue");
    //  drawLabel(1, 1, "Most recently selected menu is:");
    //  drawLabel(1 + TextStringWidth("Most recently selected menu is:"), 1, selectedLabel);
}

static void drawKeyboardPage()
{
    DisplayClear();
    SetPenColor("Tianyi"); // You can Change BackGround Color Here !
    drawRectangle(0, 0, winWidth, winHeight, 1);

    SetPenColor("Pink");
    drawRectangle(1.5, 0.3, 4.7, 4.6, 1);

    SetPenColor("Grey");
    SetPointSize(60);
    drawLabel(0.55, 5.47, "KeyBoard Shortcuts help:");
    SetPenColor("Miku");
    drawLabel(0.6, 5.5, "KeyBoard Shortcuts help:");

    MovePen(0.95, 4.9);
    SetPenColor("Yellow");
    SetPenSize(10);
    DrawLine(5.9, 0);

    SetPenColor("Cyan");

    MovePen(5, 1);
    SetPointSize(10);

    SetPointSize(23);
    drawLabel(2, 4.5, "New File");
    drawLabel(4.5, 4.5, "Ctrl+N");

    drawLabel(2, 4.1, "Open File");
    drawLabel(4.5, 4.1, "Ctrl+O");

    drawLabel(2, 3.7, "Save");
    drawLabel(4.5, 3.7, "Ctrl+S");

    drawLabel(2, 3.3, "Exit");
    drawLabel(4.5, 3.3, "Ctrl+E");

    drawLabel(2, 2.9, "Undo");
    drawLabel(4.5, 2.9, "Ctrl+Z");

    drawLabel(2, 2.5, "Redo");
    drawLabel(4.5, 2.5, "Ctrl+Y");

    drawLabel(2, 2.1, "Cut");
    drawLabel(4.5, 2.1, "Ctrl+X");

    drawLabel(2, 1.7, "Copy");
    drawLabel(4.5, 1.7, "Ctrl+C");

    drawLabel(2, 1.3, "Paste");
    drawLabel(4.5, 1.3, "Ctrl+P");

    drawLabel(2, 0.9, "Find");
    drawLabel(4.5, 0.9, "Ctrl+F");

    drawLabel(2, 0.5, "Replace:");
    drawLabel(4.5, 0.5, "Ctrl+H");

    // 来一条分割线
    SetPenSize(4);
    SetPenColor("Purple");
    MovePen(3.7, 0.4);
    DrawLine(0, 4.3);

    // 下面弄一个返回按钮
    SetPenSize(2);
    SetPenColor("Blue");
    SetPointSize(13);
    if (button(GenUIID(0), 0.3, 6.5, 1, 0.3, "Back"))
    {
        isShowKeyboard = 0;
    }
    if (!isShowKeyboard)
    {
        display();
    }
}

static void drawAboutPage()
{
    DisplayClear();

    // 画个鸟用

    SetPenColor("Tianyi"); // 这里可以改背景颜色
    drawRectangle(0, 0, winWidth, winHeight, 1);
    SetPenColor("Black"); // 把颜色设置回去

    SetPenColor("Black");
    MovePen(6.5, 5);
    DrawArc(0.8, 0, 360);
    MovePen(6.3, 5.3);
    DrawArc(0.2, 0, 360);
    MovePen(6.2, 5.3);
    DrawArc(0.05, 0, 360);

    MovePen(6.4, 5.36);
    DrawLine(0.7, 0.3);

    MovePen(6.5, 5);
    DrawLine(0.6, 0.66);

    MovePen(6.5, 5);
    DrawLine(0.6, -0.66);

    MovePen(6.4, 4.64);
    DrawLine(0.7, -0.3);

    MovePen(5.7, 4.2);
    DrawArc(2, 0, -135);
    DrawLine(3.4, 1.414);

    MovePen(3.7, 2.2);
    DrawLine(0, -1.4);
    MovePen(3.7, 1.5);
    DrawLine(-0.5, -0.7);
    MovePen(3.7, 1.5);
    DrawLine(0.5, -0.7);

    MovePen(5.0, 2.7);
    DrawLine(1, -1);
    MovePen(5.5, 2.2);
    DrawLine(0, -0.7);
    MovePen(5.5, 2.2);
    DrawLine(0.7, 0);

    MovePen(4.5, 3.7);
    DrawLine(-2, 2);
    DrawArc(2.828, 195, 60);
    MovePen(4.5, 3.7);
    DrawLine(-2.828, 0);
    DrawArc(3.7, 247.5, 45);

    int i = GetPointSize();
    int m = GetPenSize();

    SetPointSize(20);
    drawLabel(6.7, 4.9, "This TextEditor is so Amazing!!");

    // 画框框
    SetPenSize(4);
    SetPenColor("Pink");
    drawRectangle(6.8, 0.8, 2.8, 3.3, 1);

    // 写字

    // About Us
    SetPointSize(60);
    SetPenColor("Miku");
    drawLabel(6.5, 3.8, "About Us");

    // 写什么好呢
    SetPenColor("Yellow");
    SetPointSize(20);
    drawLabel(7, 3.4, "This is a TextEditor.");

    drawLabel(7, 3, "We made it in C,");
    drawLabel(7, 2.75, "also with libgraphics.");

    drawLabel(7, 2.35, "It's such a hard work,");
    drawLabel(7, 2.1, "and I hope you like it.");

    SetPenColor("Red");
    drawLabel(7.8, 1.3, "----The PHANTOM");

    // 恢复
    SetPointSize(i);
    SetPenSize(m);
    SetPenColor("Black");

    // 再整一个按钮，就也能返回了
    SetPenSize(2);
    if (button(GenUIID(0), 0.3, 6.5, 1, 0.3, "Back"))
    {
        isShowAbout = 0;
    }
    if (!isShowAbout)
    {
        display();
    }
}

/// @brief this function convert a Int number to String and Draw it on the screen.   Right aligns
/// @param num Must less than 999.
static void drawnumber(int num)
{
    int originPenSize = GetPenSize();
    static char ge[4];
    double cx, cy;
    cx = GetCurrentX();
    cy = GetCurrentY();
    ge[0] = ge[1] = ge[2] = ' ';
    ge[3] = '\0';
    if (num >= 1 && num <= 9)
    {
        MovePen(cx + 0.09, cy);
        ge[2] = '0';
        ge[2] = ge[2] + num;
    }
    else if (num >= 10 && num <= 99)
    {
        MovePen(cx + 0.045, cy);
        ge[1] = '0';
        ge[2] = '0';
        ge[1] = ge[1] + num / 10;
        ge[2] = ge[2] + num % 10;
    }
    else if (num >= 100 && num <= 999)
    {
        ge[0] = '0';
        ge[1] = '0';
        ge[2] = '0';
        ge[0] = ge[0] + num / 100;
        num %= 100;
        ge[1] = ge[1] + num / 10;
        ge[2] = ge[2] + num % 10;
    }
    DrawTextString(ge);
    SetPenSize(originPenSize);
}

static void drawSideBar()
{
    blockNode winCurrent = getWindowCurrent();
    int cps, cpointsize;
    cpointsize = GetPointSize();
    SetPointSize(13);
    cps = GetPenSize();

    SetPenColor("Pink");
    drawRectangle(0, 0, sideBarWidth - 0.05, winHeight - menuHeight - 0.01, 1);

    SetPenColor("Blue");
    SetPenSize(3);
    MovePen(sideBarWidth - 0.03, winHeight - menuHeight);
    DrawLine(0, -winHeight);

    int i;
    MovePen(0.05, winHeight - menuHeight - textBoxHeight + 0.04);
    for (i = winCurrent.row; i < winCurrent.row + maxShowR; i++)
    {
        SetPenColor("Black");
        double cx = GetCurrentX();
        double cy = GetCurrentY();

        SetPointSize(13);
        drawnumber(i + 1);
        SetPointSize(5);

        SetPenSize(2);

        if (i != winCurrent.row + maxShowR - 1)
        {
            SetPenColor("Sideline");
            MovePen(cx - 0.13, cy - 0.04);
            DrawLine(0.52, 0);
        }
        MovePen(cx, cy - textBoxHeight);
    }
    SetPenSize(cps);
    SetPointSize(cpointsize);
}

static void drawTextArea()
{
    SetPenColor("Black");
    SetPointSize(textPointSize);
    double fH = GetFontHeight();

    blockNode Cur = getCursor();
    blockNode startSelect = getStartSelect();
    blockNode endSelect = getEndSelect();
    blockNode windowCurrent = getWindowCurrent();
    gotoXLine(windowCurrent.row); // 从当前可见行开始展示
    PtrToLine curLine = getCurLine();

    int row = windowCurrent.row;
    char Word[3];
    int wordL = 0;

    if (startSelect.col != endSelect.col || startSelect.row != endSelect.row)
    {
        if (startSelect.row > endSelect.row || (startSelect.row == endSelect.row && startSelect.col > endSelect.col))
        {
            blockNode temp = startSelect;
            startSelect = endSelect;
            endSelect = temp;
        }
    }

    SetPenColor("backgroundcolor"); // 这里可以改背景颜色
    drawRectangle(sideBarWidth - 0.05, 0, winWidth - sideBarWidth + 0.05, winHeight - menuHeight, 1);
    SetPenColor("Black"); // 把颜色设置回去

    while (curLine != NULL && row < windowCurrent.row + maxShowR)
    {
        // x,y表示 下一个字符在窗口中的位置。
        double x = sideBarWidth;
        double y = winHeight - menuHeight - (row + 1 - windowCurrent.row) * textBoxHeight;
        // drawRectangle(x, y, winWidth, 0, 0);      // 画一个方框测试
        x = x - windowCurrent.col * textBoxWidth; //  这里可能会从负坐标开始画，但不该显示的部分可以被后面的sidebar覆盖掉。
        int i;

        // 绘制选中区域
        if (startSelect.row != endSelect.row)
        {
            if (row > startSelect.row && row < endSelect.row)
            {
                SetPenColor("Selected");
                drawRectangle(sideBarWidth, winHeight - menuHeight - (row - windowCurrent.row + 1) * textBoxHeight, curLine->Len * textBoxWidth - textBoxWidth, textBoxHeight, 1);
                SetPenColor("Black");
            }
            if (row == startSelect.row)
            {
                SetPenColor("Selected");
                drawRectangle(sideBarWidth + startSelect.col * textBoxWidth, winHeight - menuHeight - (row - windowCurrent.row + 1) * textBoxHeight, curLine->Len * textBoxWidth - startSelect.col * textBoxWidth - textBoxWidth, textBoxHeight, 1);
                SetPenColor("Black");
            }
            if (row == endSelect.row)
            {
                SetPenColor("Selected");
                drawRectangle(sideBarWidth, winHeight - menuHeight - (row - windowCurrent.row + 1) * textBoxHeight, endSelect.col * textBoxWidth, textBoxHeight, 1);
                SetPenColor("Black");
            }
        }

        if (startSelect.row == endSelect.row && row == startSelect.row && startSelect.col != endSelect.col)
        {
            SetPenColor("Selected");
            drawRectangle(sideBarWidth + startSelect.col * textBoxWidth, winHeight - menuHeight - (row - windowCurrent.row + 1) * textBoxHeight, (endSelect.col - startSelect.col) * textBoxWidth, textBoxHeight, 1);
            SetPenColor("Black");
        }

        for (i = 0; i < curLine->Len - 1; i++)
        {
            if (curLine->Text[i] & 0x80)
            {
                // drawRectangle(x, x, textBoxWidth, textBoxHeight, 0);
                Word[wordL++] = curLine->Text[i];
                Word[wordL++] = curLine->Text[i + 1];
                Word[wordL] = '\0';
                MovePen(x + (textBoxWidth * 2 - TextStringWidth(Word)) / 2, y);
                DrawTextString(Word);
                wordL = 0;
                x = x + textBoxWidth * 2.0;
                i++;
            }
            else
            {
                // drawRectangle(x, y, textBoxWidth * 2, textBoxHeight, 0);
                Word[wordL++] = curLine->Text[i];
                Word[wordL] = '\0';
                MovePen(x + (textBoxWidth - TextStringWidth(Word)) / 2, y);
                DrawTextString(Word);
                x = x + textBoxWidth;
                wordL = 0;
            }
        }

        drawSideBar();

        // Draw Cursor
        if (clock() % 1000 < 500)
        { // 500ms的间隔闪烁，不在输入状态时不显示光标
            int originPenSize = GetPenSize();
            SetPenSize(2);
            if (row == Cur.row)
            {
                x = sideBarWidth + (Cur.col - windowCurrent.col) * textBoxWidth;
                MovePen(x, y);
                DrawLine(0, textBoxHeight);
            }
            SetPenSize(originPenSize);
        }
        curLine = curLine->nxtNode;
        row++;
    }
    SetPenColor("Black");
}

void display()
{
    if (lockDisplay)
    {
        return;
    }
    lockDisplay = 1;
    DisplayClear();

    winHeight = GetWindowHeight();
    winWidth = GetWindowWidth();

    if (isShowAbout)
    {
        drawAboutPage();
    }
    else if (isShowKeyboard)
    {
        drawKeyboardPage();
    }
    else
    {
        drawTextArea();
        drawMenu();
    }
    lockDisplay = 0;
}