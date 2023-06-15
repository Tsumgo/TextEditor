#include <stdlib.h>
#include <stdio.h>

#include "middleware.h"
#include "model.h"
#include "strlib.h"
#include "file.h"

extern PtrToLine tailLine;
extern PtrToLine headLine;
extern int isSaved;
extern int TotalRow;
// =========================== Input And Delete =================================

// 在当前光标后插入一串内容，返回插入末尾的光标位置
blockNode inputContent(blockNode Cur, char *Buf, int bufLen) // 注意使用Cur
{
    //  Buf_example = [abc123\nabc123\nabc]
    //  模拟在键盘中输入一遍上述字符
    if (bufLen <= 0)
        return Cur;

    setSaveState(0);
    int head = 0, tail = 0;
    gotoXLine(Cur.row);
    PtrToLine curLine = getCurLine();

    PtrToLine CurToEnd = getContent(Cur, (blockNode){Cur.row, curLine->Len - 1}); // 不包括最后的0
    Erase(curLine, Cur.col, curLine->Len - 1);                                    // 不包括最后的0
    while (tail < bufLen)
    {
        if (Buf[tail] == '\n') // 回车换行了
        {
            // 插入
            Insert(curLine, Cur.col, Buf + head, tail - head); // 最后的回车不插入
            // 新建节点
            PtrToLine newLine = newLineNode();
            newLine->nxtNode = curLine->nxtNode;
            newLine->preNode = curLine;
            curLine->nxtNode = newLine;
            // 更新最后一行的节点
            if (curLine == tailLine)
                tailLine = newLine;

            // 更新总行数
            TotalRow++;
            // 更新鼠标位置
            Cur = (blockNode){Cur.row + 1, 0};
            curLine = curLine->nxtNode;
            head = tail + 1;
        }
        tail++;
    }

    Insert(curLine, Cur.col, Buf + head, tail - head);
    Cur = (blockNode){Cur.row, Cur.col + tail - head};
    Insert(curLine, Cur.col, CurToEnd->Text, CurToEnd->Len - 1);
    free(CurToEnd->Text), free(CurToEnd);
    return Cur;
}

// 从start -> end 删除，移动光标到start位置
void deleteContent(blockNode start, blockNode end)
{
    if (start.row > end.row || (start.row == end.row && start.col > end.col))
    {
        blockNode t = start;
        start = end;
        end = t;
    }
    if (start.row == end.row && start.col == end.col)
        return;

    setSaveState(0);
    PtrToLine curLine;
    if (start.row == end.row)
    {
        gotoXLine(start.row);
        curLine = getCurLine();
        Erase(curLine, start.col, end.col);
    }
    else
    {
        // 删除end前的字符
        gotoXLine(end.row);
        curLine = getCurLine();
        Erase(curLine, 0, end.col);

        // 删除start后的字符
        gotoXLine(start.row);
        curLine = getCurLine();
        Erase(curLine, start.col, curLine->Len);

        // curLine 在start.row上
        int len = end.row - start.row - 1;
        // 删除start和end之间的行
        for (int i = 1; i <= len; i++)
        {
            deleteLineNode(curLine->nxtNode);
        }

        // 将end所在行的字符插入到start行
        len = curLine->nxtNode->Len;
        for (int i = 0; i < len; i++)
        {
            char ch = curLine->nxtNode->Text[i];
            PushBack(curLine, ch);
        }

        // 删除原来的end行
        deleteLineNode(curLine->nxtNode);
    }
}

// 返回 start -> end 的文本内容  记得free
PtrToLine getContent(blockNode startSelect, blockNode endSelect)
{
    gotoXLine(startSelect.row);
    PtrToLine curLine = getCurLine();
    int i, rowlenth;
    PtrToLine dst = newLineNode();

    if (startSelect.row == endSelect.row)
    {
        Insert(dst, dst->Len - 1, curLine->Text + startSelect.col, endSelect.col - startSelect.col);
        return dst;
    }
    rowlenth = endSelect.row - startSelect.row + 1;
    for (i = 0; i < rowlenth; i++)
    {
        if (i == 0)
        {
            Insert(dst, dst->Len - 1, curLine->Text + startSelect.col, curLine->Len - startSelect.col - 1);
            Insert(dst, dst->Len - 1, "\n", 1); // 去掉行末的 \0 改为回车
        }
        else if (i == rowlenth - 1)
        {
            Insert(dst, dst->Len - 1, curLine->Text, endSelect.col);
        }
        else
        {
            Insert(dst, dst->Len - 1, curLine->Text, curLine->Len - 1); // 去掉末尾的 \0 改为回车
            Insert(dst, dst->Len - 1, "\n", 1);
        }
        curLine = curLine->nxtNode;
    }
    return dst;
}

PtrToLine getContentFromBuf(char *Buf, int bufLen)
{
    PtrToLine dst = newLineNode();
    Insert(dst, 0, Buf, bufLen);
    return dst;
}
