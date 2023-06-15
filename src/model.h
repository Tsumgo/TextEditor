#ifndef model_h
#define model_h

/// @brief A struct like vector
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
} blockNode;

void Insert(PtrToLine line, int pos, char *Buf, int bufLen);

void PushBack(PtrToLine line, char x);

void Erase(PtrToLine line, int start, int end);

PtrToLine newLineNode();

void deleteLineNode();

void changeLine();

void printAllLine();

void gotoXLine(int x);
// ============= List ==========================

int getTotalRow();

blockNode getCursor();

blockNode getStartSelect();

blockNode getEndSelect();

PtrToLine getCurLine();

blockNode getWindowCurrent();

void setCurLine(PtrToLine new_curLine);

void setCursor(blockNode new_cursor);

void setStartSelect(blockNode new_startSelect);

void setEndSelect(blockNode new_endSelect);

void updateWinCurrent(blockNode Cur);

/// @brief  set Cursor to (0,0) and clear all text contents.
void initModel();
#endif