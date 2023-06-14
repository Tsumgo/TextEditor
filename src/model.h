#ifndef model_h
#define model_h

typedef struct lineNode
{
    char *Text;
    struct lineNode *preNode;
    struct lineNode *nxtNode;
    int Len;
    int Cap;
} lineNode;
typedef lineNode *PtrToLine;

typedef struct blockNode
{
    int row;
    int col;
} blockNode; // which row and column the cursor at .

void Insert(PtrToLine line, int pos, char *Buf, int bufLen);

void PushBack(PtrToLine line, char x);

void Erase(PtrToLine line, int start, int end);

PtrToLine newLineNode(); //  End with '\0'

void deleteLineNode();

void changeLine();

void printAllLine();

void gotoXLine(int x);
// ============= List ==========================
int getTotalRow(); //****链表跑一遍，计算当前有多少行

blockNode getCursor(); //****

blockNode getStartSelect(); //****

blockNode getEndSelect(); //****

PtrToLine getCurLine(); //****

blockNode getWindowCurrent();

void setCurLine(PtrToLine new_curLine); //****

void setCursor(blockNode new_cursor); //****

void setStartSelect(blockNode new_startSelect); //****

void setEndSelect(blockNode new_endSelect); //****

void updateWinCurrent(blockNode Cur);

void initModel();
#endif