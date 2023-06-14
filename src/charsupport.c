#include <windows.h>
#include <winuser.h>

#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"

#include "charsupport.h"
#include "middleware.h"
#include "model.h"
#include "display.h"
#include "edit.h"

static blockNode Cur;
static blockNode startSelect;
static blockNode endSelect;

static char Word[5];
static int wordL = 0;
extern Stack Undo, Redo;

// 区分当前输入字符
int Divide(char ch)
{
    if (ch & 0x80)
        return 2; // 中文
    if (ch >= 32 && ch <= 126)
        return 1; // 可见字符
    // if (ch == 10)
    // return 0; // 回车
    if (getUIState().keyPress == VK_RETURN)
        return 0;
    return -1;
}
void CharEventProcess(char ch)
{
    uiGetChar(ch);
    Cur = getCursor();
    startSelect = getStartSelect();
    endSelect = getEndSelect();

    int opt = Divide(ch);
    // printf("Option :%d\n", opt);
    switch (opt)
    {
    case 2: // 中文
        Word[wordL++] = ch;
        Word[wordL] = '\0';
        break;
    case 1: // 其他可见字符
        Word[wordL++] = ch;
        Word[wordL] = '\0';
        break;
    case 0: // 回车
        Word[wordL++] = '\n';
        Word[wordL] = '\0';
        break;
    default: // 控制字符, 直接退出函数
        return;
        break;
    }
    // if (opt == 0)
    //     puts("retur 1 ");
    if (opt == 0 || opt == 1 || (opt == 2 && wordL == 2))
    {
        if ((startSelect.col != endSelect.col || startSelect.row != endSelect.row))
        {
            if (startSelect.row > endSelect.row || (startSelect.row == endSelect.row && startSelect.col > endSelect.col))
            {
                blockNode temp = startSelect;
                startSelect = endSelect;
                endSelect = temp;
            }
            PtrToLine delBuf = getContent(startSelect, endSelect);
            PtrToRecord event = newRecord(2, Cur, startSelect, endSelect, startSelect, endSelect, delBuf);
            stkPush(&Undo, event);

            deleteContent(startSelect, endSelect);
            Cur = startSelect; // 重定位光标
            endSelect = startSelect;
        }
        blockNode inputEnd = inputContent(Cur, Word, wordL);
        PtrToLine inpBuf = getContentFromBuf(Word, wordL);
        PtrToRecord event = newRecord(1, Cur, startSelect, endSelect, Cur, inputEnd, inpBuf);
        // printf("Cursor: %d %d; Select: %d %d to %d %d\n", ().row, getCursor().col, getStartSelect().row, getStartSelect().col, getEndSelect().row, getEndSelect().col);

        stkPush(&Undo, event);
        wordL = 0;
        stkClear(&Redo);

        setCursor(inputEnd);
        setStartSelect(inputEnd);
        setEndSelect(inputEnd);
    }

    printAllLine();
    display();
}
