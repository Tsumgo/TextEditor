#include <windows.h>
#include <winuser.h>

#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"

#include "charsupport.h"
#include "middleware.h"
#include "model.h"
#include "display.h"
#include "edit.h"

static blockNode Cur;
static blockNode startSelect;
static blockNode endSelect;

static char Word[5];
static int wordL = 0;
extern Stack Undo, Redo;

/// @brief this fucntion determine What current character is
/// @param ch the input character
/// @return if ch is a Chinese, return 2; if it's visible ASCII, return 1; if it's a  RETURN, return 0;
static int Divide(char ch);

static int Divide(char ch)
{
    if (ch & 0x80)
        return 2;
    if (ch >= 32 && ch <= 126)
        return 1;
    if (getUIState().keyPress == VK_RETURN)
        return 0;
    return -1;
}
void CharEventProcess(char ch)
{
    uiGetChar(ch);
    Cur = getCursor();
    startSelect = getStartSelect();
    endSelect = getEndSelect();

    int opt = Divide(ch);
    switch (opt)
    {
    case 2:
        Word[wordL++] = ch;
        Word[wordL] = '\0';
        break;
    case 1:
        Word[wordL++] = ch;
        Word[wordL] = '\0';
        break;
    case 0:
        Word[wordL++] = '\n';
        Word[wordL] = '\0';
        break;
    default:
        return;
        break;
    }
    if (opt == 0 || opt == 1 || (opt == 2 && wordL == 2))
    {
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
            Cur = startSelect;
            endSelect = startSelect;
        }
        blockNode inputEnd = inputContent(Cur, Word, wordL);
        PtrToLine inpBuf = getContentFromBuf(Word, wordL);
        PtrToRecord event = newRecord(1, Cur, startSelect, endSelect, Cur, inputEnd, inpBuf);

        stkPush(&Undo, event);
        wordL = 0;
        stkClear(&Redo);

        setCursor(inputEnd);
        setStartSelect(inputEnd);
        setEndSelect(inputEnd);
    }
    display();
}
