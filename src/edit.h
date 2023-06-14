#ifndef _EDIT_H
#define _EDIT_H

#include "model.h"

typedef struct editHistory
{
    PtrToLine Buf;
    blockNode Cur;         // 光标位置
    blockNode startSelect; // 定义操作前的选中位置。没有则和Cur相同
    blockNode endSelect;
    blockNode OpStart; // 操作的开始节点
    blockNode OpEnd;   // 操作的结束节点
    int Option;        // 操作指令,0:单独处理对被删除选中区域的还原  1: 增加； 2： 删除
} record;
/*
// 1 在Cur位置插入 , del = NULL;  startSelect = endSelect = Cur;   input->Buf 存放插入内容
        撤销时，先deleteContent   but from Where To Where ?

*/
typedef record *PtrToRecord;

#define stackCap 25 // 栈容量
typedef struct Stack
{ // 堆栈，存放历史操作的指针
    int Top;
    PtrToRecord Stk[50];
} Stack;

void stkPush(Stack *stk, PtrToRecord record);

void stkPop(Stack *stk);

void UnDo(); ///

void ReDo(); ///

void Copy(blockNode startSelect, blockNode endSelect);

void Paste();

PtrToRecord stkTop(Stack *stk);

PtrToRecord newRecord(int opt, blockNode Cur, blockNode startSelect, blockNode endSelect, blockNode OpStart, blockNode Opend, PtrToLine Buf);

void stkClear(Stack *stk);

#endif
