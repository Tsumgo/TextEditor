#include <windows.h>
#include <winuser.h>

#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"

#include "keyboardsupport.h"
#include "middleware.h"
#include "model.h"
#include "display.h"
#include "edit.h"

static blockNode Cur;
static blockNode startSelect;
static blockNode endSelect;
static PtrToLine curLine;
static int TotalRow;
static UIState UI;
static blockNode winCurrent;

extern Stack Redo, Undo;
extern PtrToLine headLine;
extern PtrToLine tailLine;

static blockNode Succ(blockNode now)
{
    // blockNode succ;
    if (now.row == TotalRow - 1 && now.col == tailLine->Len - 1) // 文末
        return now;
    if (now.col == curLine->Len - 1) // 行末
        return (blockNode){now.row + 1, 0};
    // 是不是中文
    return (curLine->Text[now.col] & 0x80) ? (blockNode){now.row, now.col + 2} : (blockNode){now.row, now.col + 1};
}

static blockNode Pre(blockNode now)
{
    // blockNode Pre;
    if (now.row == 0 && now.col == 0) // 文首
        return now;
    if (now.col == 0) // 行首
        return (blockNode){now.row - 1, curLine->preNode->Len - 1};
    return (curLine->Text[now.col - 1] & 0x80) ? (blockNode){now.row, now.col - 2} : (blockNode){now.row, now.col - 1};
}

static void TurnBack()
{
    if ((startSelect.col != endSelect.col || startSelect.row != endSelect.row) && UI.keyModifiers != 1)
    { // 当前有选中范围并且没按下shift，把光标移到前端
        if (startSelect.row > endSelect.row || (startSelect.row == endSelect.row && startSelect.col > endSelect.col))
        {
            blockNode temp = startSelect;
            startSelect = endSelect;
            endSelect = temp;
        }
        setStartSelect(startSelect);
        setEndSelect(startSelect);
        setCursor(startSelect);
        return;
    }
    blockNode preCur = Pre(Cur);
    setCursor(preCur);
    if (UI.keyModifiers == 1) // Shift : 1
    {
        setEndSelect(preCur);
    }
    else
    {
        setStartSelect(preCur);
        setEndSelect(preCur);
    }
    updateWinCurrent(preCur);
}

static void TurnForward()
{
    int len = curLine->Len;

    if ((startSelect.col != endSelect.col || startSelect.row != endSelect.row) && UI.keyModifiers != 1)
    { // 当前有选中范围并且没按下shift，把光标移到选中范围的末尾 + 1
        if (startSelect.row > endSelect.row || (startSelect.row == endSelect.row && startSelect.col > endSelect.col))
        {
            blockNode temp = startSelect;
            startSelect = endSelect;
            endSelect = temp;
        }

        setStartSelect(endSelect);
        setEndSelect(endSelect);
        setCursor(endSelect);
        return;
    }
    blockNode nxtCur = Succ(Cur);
    setCursor(nxtCur);
    if (UI.keyModifiers == 1) // Shift : 1
    {
        setEndSelect(nxtCur);
    }
    else
    {
        setStartSelect(nxtCur);
        setEndSelect(nxtCur);
    }
    updateWinCurrent(nxtCur);
}

static void TurnUp()
{

    if (Cur.row == 0)
    { // 光标位于第一行，移到开头
        if (UI.keyModifiers == 1)
        {
            setCursor((blockNode){0, 0});
            setStartSelect((blockNode){0, 0});
            setEndSelect(Cur);
        }
        else
        {
            setStartSelect(Cur);
            setEndSelect(Cur);
        }
    }
    else
    {
        curLine = curLine->preNode;
        int i = 0;
        int len = curLine->Len;
        int flag = 0;
        while (i < len && curLine->Text[i] != '\n')
        {
            if (Cur.col == i || (Cur.col == i + 1 && curLine->Text[i] & 128))
            { // 如果遇到中文字符
                Cur.col = i;
                Cur.row--;
                setCursor(Cur);
                if (UI.keyModifiers == 1)
                {
                    setEndSelect(Cur);
                }
                else
                {
                    setStartSelect(Cur);
                    setEndSelect(Cur);
                }
                flag = 1;
                break;
            }
            if (curLine->Text[i] & 128)
                i++; // 处理中文
            i++;
        }
        if (!flag)
        { // 把光标移到上一行行末
            setCursor((blockNode){Cur.row - 1, len - 1});
            if (UI.keyModifiers == 1)
            {
                setEndSelect((blockNode){Cur.row - 1, len - 1});
            }
            else
            {
                setEndSelect((blockNode){Cur.row - 1, len - 1});
                setStartSelect((blockNode){Cur.row - 1, len - 1});
            }
        }
    }
    updateWinCurrent(getCursor());
}

static void TurnDown()
{

    if (Cur.row == TotalRow - 1)
    {
        if (UI.keyModifiers == 1)
        {
            setCursor((blockNode){Cur.row, curLine->Len - 1});
            setStartSelect(Cur);
            setEndSelect((blockNode){Cur.row, curLine->Len - 1});
        }
        else
        {
            setStartSelect(Cur);
            setEndSelect(Cur);
        }
    }
    else
    {
        int i = 0;
        int flag = 0;
        curLine = curLine->nxtNode;
        int len = curLine->Len;
        if (len >= 1 && (curLine->Text[len - 1] == '\0' || curLine->Text[len - 1] == '\n'))
            len--;
        while (i < len)
        {
            if (Cur.col == i || (Cur.col == i + 1 && curLine->Text[i] & 128))
            {
                Cur.col = i;
                Cur.row++;
                setCursor(Cur);
                if (UI.keyModifiers == 1)
                {
                    setEndSelect(Cur);
                }
                else
                {
                    setStartSelect(Cur);
                    setEndSelect(Cur);
                }
                flag = 1;
                break;
            }
            if (curLine->Text[i] & 128)
                i++; // 处理中文
            i++;
        }
        if (!flag)
        { // 移动到下一行行末
            setCursor((blockNode){Cur.row + 1, len});
            if (UI.keyModifiers == 1)
            {
                setEndSelect((blockNode){Cur.row + 1, len});
            }
            else
            {
                setStartSelect((blockNode){Cur.row + 1, len});
                setEndSelect((blockNode){Cur.row + 1, len});
            }
        }
    }
    updateWinCurrent(getCursor());
}

static void BackSpace()
{
    if ((startSelect.col != endSelect.col || startSelect.row != endSelect.row))
    {
        if (startSelect.row > endSelect.row || (startSelect.row == endSelect.row && startSelect.col > endSelect.col))
        {
            blockNode temp = startSelect;
            startSelect = endSelect;
            endSelect = temp;
        }
        PtrToLine delBuf = getContent(startSelect, endSelect);                                           // malloc
        PtrToRecord newReco = newRecord(2, Cur, startSelect, endSelect, startSelect, endSelect, delBuf); // malloc
        stkPush(&Undo, newReco);

        deleteContent(startSelect, endSelect);
        setCursor(startSelect);
        setStartSelect(startSelect);
        setEndSelect(startSelect);
        return;
    }

    if (Cur.row == 0 && Cur.col == 0)
        return;

    blockNode delStart, delEnd;
    delEnd = Cur;
    delStart = Pre(Cur);
    if (Cur.col == 0)
    {
        delStart = (blockNode){Cur.row - 1, curLine->preNode->Len - 1}; // 上一个节点的末尾
    }
    else if ((curLine->Text[Cur.col - 1] & 0x80))
    {
        delStart = (blockNode){Cur.row, Cur.col - 2};
    }
    else
        delStart = (blockNode){Cur.row, Cur.col - 1};

    PtrToLine delBuf = getContent(delStart, delEnd); // malloc
    PtrToRecord newReco = newRecord(2, Cur, startSelect, endSelect, delStart, delEnd, delBuf);
    stkPush(&Undo, newReco);

    deleteContent(delStart, delEnd);
    setCursor(delStart);
    setStartSelect(delStart);
    setEndSelect(delStart);
}

static void Delete()
{
    if ((startSelect.col != endSelect.col || startSelect.row != endSelect.row))
    {
        if (startSelect.row > endSelect.row || (startSelect.row == endSelect.row && startSelect.col > endSelect.col))
        {
            blockNode temp = startSelect;
            startSelect = endSelect;
            endSelect = temp;
        }
        PtrToLine delBuf = getContent(startSelect, endSelect);                                           // malloc
        PtrToRecord newReco = newRecord(2, Cur, startSelect, endSelect, startSelect, endSelect, delBuf); // malloc
        stkPush(&Undo, newReco);

        deleteContent(startSelect, endSelect);
        setCursor(startSelect);
        setStartSelect(startSelect);
        setEndSelect(startSelect);
        return;
    }

    if (Cur.row == TotalRow - 1 && Cur.col == curLine->Len - 1) // 文末
        return;

    blockNode delStart, delEnd;
    delStart = Cur;
    delEnd = Succ(Cur);

    PtrToLine delBuf = getContent(delStart, delEnd); // malloc
    PtrToRecord newReco = newRecord(2, Cur, startSelect, endSelect, delStart, delEnd, delBuf);
    stkPush(&Undo, newReco);

    deleteContent(delStart, delEnd);
    setCursor(delStart);
    setStartSelect(delStart);
    setEndSelect(delStart);
}

void KeyboardEventProcess(int key, int event)
{
    uiGetKeyboard(key, event);

    Cur = getCursor();
    startSelect = getStartSelect();
    endSelect = getEndSelect();
    gotoXLine(Cur.row);
    curLine = getCurLine();
    TotalRow = getTotalRow();
    winCurrent = getWindowCurrent();
    UI = getUIState();

    if (event == KEY_DOWN)
    {
        switch (key)
        {
        case VK_LEFT:
            TurnBack();
            break;
        case VK_RIGHT:
            TurnForward();
            break;
        case VK_UP:
            TurnUp();
            break;
        case VK_DOWN:
            TurnDown();
            break;
        case VK_BACK:
            BackSpace();
            break;
        case VK_DELETE:
            Delete();
            break;
        }
        display();
    }
}