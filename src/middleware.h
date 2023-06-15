#ifndef _MIDDLEWARE_H
#define _MIDDLEWARE_H
#include "model.h"
#include "strlib.h"

/// @brief  input Text Content in the TextEditor
/// @param Cur Start at Cur
/// @param Buf Characters ready to input
/// @param bufLen Length of Buf
/// @return the Cursor position after inputing all char in Buf
blockNode inputContent(blockNode Cur, char *Buf, int bufLen);

/// @brief Delete Text Content from Start to End.
/// @param start the Start position of a delete
/// @param end the End position of a delete
void deleteContent(blockNode start, blockNode end);

/// @brief this function returns all characters in the Text from Start to End
/// @param startSelect start select position
/// @param endSelect end select position
/// @return a PtrToLine type. PtrToLine->Text Stores the text we need
PtrToLine getContent(blockNode startSelect, blockNode endSelect);

PtrToLine getContentFromBuf(char *Buf, int bufLen);

#endif