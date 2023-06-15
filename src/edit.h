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
typedef record *PtrToRecord;

#define stackCap 50 // 栈容量
typedef struct Stack
{ // 堆栈，存放历史操作的指针
    int Top;
    PtrToRecord Stk[50];
} Stack;

void initEdit();

void stkPush(Stack *stk, PtrToRecord record);

void stkPop(Stack *stk);

void UnDo();

void ReDo();

void Copy();

void Paste();

void Cut();

PtrToRecord stkTop(Stack *stk);

PtrToRecord newRecord(int opt, blockNode Cur, blockNode startSelect, blockNode endSelect, blockNode OpStart, blockNode Opend, PtrToLine Buf);

void stkClear(Stack *stk);

#endif
