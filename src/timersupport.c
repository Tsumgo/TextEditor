#include <windows.h>
#include <winuser.h>
#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"

#include "timersupport.h"
#include "display.h"

extern int isShowCursor;

#define REFREASH 1

// Control cursor flash
void TimerEventProcess(int timerID)
{
    if (timerID == REFREASH)
    {
        isShowCursor = !isShowCursor;
        display();
    }
}