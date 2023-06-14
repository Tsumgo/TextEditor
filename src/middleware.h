#ifndef _MIDDLEWARE_H
#define _MIDDLEWARE_H
#include "model.h"
#include "strlib.h"

blockNode inputContent(blockNode Cur, char *Buf, int bufLen); // 注意使用Cur

void deleteContent(blockNode start, blockNode end);

PtrToLine getContent(blockNode startSelect, blockNode endSelect);

PtrToLine getContentFromBuf(char *Buf, int bufLen);

#endif