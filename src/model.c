#include <windows.h>
#include <winuser.h>
#include <math.h>

#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"

#include "model.h"

#define _DEBUG

const int MAXLEN = 500;

static blockNode cursor;        // cursor position
static blockNode startSelect;   // selected area start point
static blockNode endSelect;     // selected area end point
static PtrToLine curLine;       // the line we focus on
static blockNode windowCurrent; // 当前展示文本左上角的行列坐标，这部分的更新还没有写

// Global Variables;
PtrToLine headLine = NULL;
PtrToLine tailLine = NULL;
int TotalRow = 0;

extern int maxShowR;
extern int maxShowC;
//=============================== Vector =============================

// Insert string on curLine
void Insert(PtrToLine line, int pos, char *Buf, int bufLen)
{
    if (bufLen <= 0)
        return;

    int newLenth = line->Len + bufLen;
    int minCapacity = 1;
    while (minCapacity < newLenth)
    {
        minCapacity <<= 1;
    }
    if (line->Cap == 0)
        minCapacity = 4;

    if (minCapacity >= line->Cap)
    {
        int newCapacity = minCapacity;
        line->Text = (char *)realloc(line->Text, newCapacity * sizeof(char));
        line->Cap = newCapacity;
        if (line->Text == NULL)
        {
            puts("Realloc Fail");
            exit(-1);
        }
    }
    for (int i = line->Len - 1; i >= pos; i--)
    {
        line->Text[i + bufLen] = line->Text[i];
    }
    for (int i = 0; i < bufLen; i++)
    {
        line->Text[pos + i] = Buf[i];
    }
    line->Len = newLenth;
}
void PushBack(PtrToLine line, char x)
{
    if (line->Len == line->Cap)
    {
        int newCapacity = line->Cap == 0 ? 4 : line->Cap * 2;
        line->Text = (char *)realloc(line->Text, newCapacity * sizeof(char));
        line->Cap = newCapacity;
        if (line->Text == NULL)
        {
            printf("Realloc Fail \n");
            exit(-1);
        }
    }
    line->Text[line->Len] = x;
    line->Len++;
}

// Erase curLine from start to end-1
void Erase(PtrToLine line, int start, int end)
{
    if (start == end)
        return;
    for (int i = end; i < line->Len; i++)
    {
        line->Text[i - (end - start)] = line->Text[i];
    }
    line->Len -= end - start;

    if (line->Cap > 4 && line->Len <= line->Cap / 2)
    {
        int NewCapacity = line->Cap / 2;
        line->Text = (char *)realloc(line->Text, NewCapacity * sizeof(char));
        line->Cap = NewCapacity;
        if (line->Text == NULL)
        {
            puts("Realloc Fail");
            exit(-1);
        }
    }
}

// ========================== List ===========================
PtrToLine newLineNode()
{
    PtrToLine line = (PtrToLine)malloc(sizeof(lineNode));
    if (line == NULL)
    {
        puts("Create new Line Node Fail");
        exit(-1);
    }
    *line = (lineNode){NULL, NULL, NULL, 0, 0};
    PushBack(line, '\0'); // 行末预置 \0
    return line;
}

void deleteLineNode(PtrToLine line)
{
    if (line == headLine)
    {
        if (TotalRow == 1) // 只有一行，不能删除
            return;
        line->nxtNode->preNode = NULL;
        TotalRow--;
        headLine = line->nxtNode;
        free(line);
        return;
    }
    if (line == tailLine)
    { // 尾行，更新行末
        line->preNode->nxtNode = NULL;
        TotalRow--;
        tailLine = line->preNode;
        free(line);
        return;
    }
    line->preNode->nxtNode = line->nxtNode;
    line->nxtNode->preNode = line->preNode;
    TotalRow--;
    free(line);
    return;
}

// 在当前光标位置换行，所以没有传递cursor参数
void changeLine()
{
    gotoXLine(cursor.row);
    PtrToLine newLine = newLineNode();
    newLine->nxtNode = curLine->nxtNode;
    newLine->preNode = curLine;
    curLine->nxtNode = newLine;
    // 更新最后一行的节点
    if (curLine == tailLine)
        tailLine = newLine;
    // 更新总行数
    TotalRow++;

    Insert(newLine, 0, curLine->Text + cursor.col, curLine->Len - cursor.col - 1); // 行末 \0 不复制
    Erase(curLine, cursor.col, curLine->Len - 1);                                  // 行末 \0 不删除
    // 更新鼠标位置
    setCursor((blockNode){cursor.row + 1, 0});
    setStartSelect(cursor);
    setEndSelect(cursor);
}

// Debug
void printAllLine()
{
    int x = 0;
    curLine = headLine;
    while (curLine != NULL)
    {
        if (x == cursor.row)
        {
            printf("Line %d : ", x);
            for (int i = 0; i < cursor.col; i++)
                printf("%c", curLine->Text[i]);
            printf("|");
            printf("%s\n", curLine->Text + cursor.col);
        }
        else
            printf("Line %d : %s\n", x, curLine->Text);
        x++, curLine = curLine->nxtNode;
    }
}

void gotoXLine(int x)
{
    curLine = headLine;
    while (x-- && curLine != tailLine)
    {
        curLine = curLine->nxtNode;
    }
}

// *************************Set & Get ****************************

//****链表跑一遍，计算当前有多少行
int getTotalRow()
{
    return TotalRow;
}

void updateWinCurrent(blockNode Cur)
{
    if (Cur.row < windowCurrent.row || Cur.row > windowCurrent.row + maxShowR - 1)
    {
        windowCurrent.row = Cur.row < windowCurrent.row ? Cur.row : Cur.row - maxShowR + 1;
    }
    if (Cur.col < windowCurrent.col || Cur.col > windowCurrent.col + maxShowC - 1)
    {
        windowCurrent.col = Cur.col < windowCurrent.col ? Cur.col : Cur.col - maxShowC + 1;
    }
}

void setCursor(blockNode new_cursor)
{
    cursor = new_cursor;
    updateWinCurrent(cursor);
}

void setStartSelect(blockNode new_startSelect)
{
    startSelect = new_startSelect;
}

void setEndSelect(blockNode new_endSelect)
{
    endSelect = new_endSelect;
}

void setCurLine(PtrToLine new_curLine)
{
    curLine = new_curLine;
}

PtrToLine getCurLine()
{
    return curLine;
}

blockNode getCursor()
{
    return cursor;
}
blockNode getStartSelect()
{
    return startSelect;
}

blockNode getEndSelect()
{
    return endSelect;
}
blockNode getWindowCurrent()
{
    return windowCurrent;
}
// ====================== init Model ========================
void initModel()
{
    setCursor((blockNode){0, 0});
    setStartSelect((blockNode){0, 0});
    setEndSelect((blockNode){0, 0});
    windowCurrent = (blockNode){0, 0};

    if (headLine != NULL)
    {
        while (headLine->nxtNode != NULL)
            deleteLineNode(headLine->nxtNode);
        free(headLine->Text), free(headLine);
    }
    tailLine = curLine = headLine = newLineNode();
    TotalRow = 1;
}