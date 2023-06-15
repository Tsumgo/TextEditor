#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "edit.h"
#include "middleware.h"
#include "model.h"

Stack Redo, Undo;

static PtrToLine CopyText; // 用于复制粘贴存放字符串

// 下列函数全部用指针传入
// 在栈 stk中插入一个记录
void initEdit()
{
    stkClear(&Redo), stkClear(&Undo);
    Redo.Top = Undo.Top = 0;
    CopyText = NULL;
}

void stkPush(Stack *stk, PtrToRecord record)
{
    int i;
    if (stk->Top == stackCap)
    {
        for (i = 0; i < stk->Top - 1; i++)
        {
            stk->Stk[i] = stk->Stk[i + 1];
        }
        stk->Stk[stk->Top - 1] = record;
    }
    else
    {
        stk->Stk[stk->Top++] = record;
    }
}

// 弹出栈首的记录
void stkPop(Stack *stk)
{
    if (stk->Top)
    {
        stk->Stk[stk->Top - 1] = NULL; // 并没有释放 里面的 Ptr To Record
        stk->Top--;
    }
}

void destroyRecord(PtrToRecord reco)
{
    free(reco->Buf->Text);
    free(reco->Buf);
    free(reco);
}
void stkClear(Stack *stk)
{
    while (stk->Top)
    {
        destroyRecord(stk->Stk[stk->Top - 1]);
        stk->Top--;
    }
}

// 得到栈首的记录，栈为空则返回NULL
PtrToRecord stkTop(Stack *stk)
{
    if (stk->Top)
        return stk->Stk[stk->Top - 1];
    return NULL;
}

// 新建一个记录
PtrToRecord newRecord(int opt, blockNode Cur, blockNode startSelect, blockNode endSelect, blockNode OpStart, blockNode OpEnd, PtrToLine Buf)
{
    PtrToRecord newReco = (PtrToRecord)malloc(sizeof(record));
    newReco->Option = opt;
    newReco->Cur = Cur;
    newReco->startSelect = startSelect;
    newReco->endSelect = endSelect;
    newReco->OpStart = OpStart;
    newReco->OpEnd = OpEnd;
    newReco->Buf = Buf;
    return newReco;
}

// 撤销
void UnDo()
{
    PtrToRecord event = stkTop(&Undo);
    if (event == NULL)
        return;
    stkPop(&Undo);

    blockNode inputEnd;
    switch (event->Option)
    {
    case 1:                                          // 撤销插入操作
        deleteContent(event->OpStart, event->OpEnd); // 插入的删除

        setCursor(event->Cur);
        setStartSelect(event->startSelect);
        setEndSelect(event->endSelect);
        stkPush(&Redo, event);
        break;
    case 2:                                                                             // 撤销删除操作
        inputEnd = inputContent(event->OpStart, event->Buf->Text, event->Buf->Len - 1); // 删除的插入

        setCursor(event->Cur);
        setStartSelect(event->startSelect); // 删除前的选中
        setEndSelect(event->endSelect);
        stkPush(&Redo, event);
        break;
    }
    printAllLine();
}

// 重做
void ReDo()
{
    PtrToRecord event = stkTop(&Redo);
    if (event == NULL)
    {
        return;
    }
    stkPop(&Redo);
    blockNode inputEnd;
    switch (event->Option)
    {
    case 1:
        inputEnd = inputContent(event->OpStart, event->Buf->Text, event->Buf->Len - 1); // 插入

        setCursor(inputEnd);      // 插入后的光标
        setStartSelect(inputEnd); // event->OpStart = Opstart;
        setEndSelect(inputEnd);   // event->OpEnd = inputEnd;
        stkPush(&Undo, event);
        break;
    case 2:                                          // 删除
        deleteContent(event->OpStart, event->OpEnd); // 删除

        setCursor(event->OpStart);
        setStartSelect(event->OpStart);
        setEndSelect(event->OpStart);
        stkPush(&Undo, event);
        break;
    }
}

void Copy(blockNode startSelect, blockNode endSelect)
{
    if ((startSelect.col != endSelect.col || startSelect.row != endSelect.row))
    {
        if (startSelect.row > endSelect.row || (startSelect.row == endSelect.row && startSelect.col > endSelect.col))
        {
            blockNode temp = startSelect;
            startSelect = endSelect;
            endSelect = temp;
        }
        free(CopyText->Text), free(CopyText);
        CopyText = getContent(startSelect, endSelect);
    }
}

void Paste()
{
    blockNode startSelect = getStartSelect();
    blockNode endSelect = getEndSelect();
    blockNode Cur = getCursor();
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

    blockNode intputEnd = inputContent(Cur, CopyText->Text, CopyText->Len - 1);

    PtrToLine inpBuf = getContentFromBuf(CopyText->Text, CopyText->Len - 1);
    PtrToRecord event = newRecord(1, Cur, startSelect, endSelect, Cur, intputEnd, inpBuf);
    stkPush(&Undo, event);
}
