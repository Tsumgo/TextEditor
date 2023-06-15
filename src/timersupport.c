#include <windows.h>
#include <winuser.h>

#include "timersupport.h"
#include "display.h"

#define REFREASH 1

// Control cursor flash
void TimerEventProcess(int timerID)
{
    if (timerID == REFREASH)
    {
        display();
    }
}