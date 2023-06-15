#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>

#include "middleware.h"
#include "imgui.h"
#include "model.h"
#include "edit.h"

extern PtrToLine headLine;
extern PtrToLine tailLine;

static FILE *currentFile;
OPENFILENAMEA ofn;
static char szFile[512];
static char szFileTitle[512];
static int isSaved;
static int isCreated;
static char msg[1010];
static int isProcessFile = 0;

int getFileProcessState()
{
    return isProcessFile;
}

int getSaveState()
{
    return isSaved;
}
void setSaveState(int newSaveState)
{
    isSaved = newSaveState;
}

void initFileConfig()
{
    isSaved = 1;
    isCreated = 0;
    ZeroMemory(&ofn, sizeof(ofn));
}

void newFile()
{
    if (isProcessFile)
        return;
    isProcessFile = 1;
    int state;
    if (!isSaved)
    {
        if (!isCreated)
        {
            sprintf(msg, "是否要保存对 无标题 的更改？");
        }
        else
        {
            sprintf(msg, "是否要保存对 %s 的更改？", ofn.lpstrFileTitle);
        }
        state = MessageBoxA(NULL, msg, "TextEditor", MB_YESNOCANCEL | MB_ICONWARNING);
        switch (state)
        {
        case IDYES:
            saveFile();
            break;
        case IDNO:
            break;
        case IDCANCEL:
            isProcessFile = 0;
            return;
        }
    }
    InitGUI();
    initModel();
    initEdit();
    isProcessFile = 0;
}

void saveFile()
{
    isProcessFile = 1;
    if (isSaved)
        return;
    if (!isCreated)
    {
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "文本文档(*.txt)\0*.txt\0所有文件(*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = szFileTitle;
        ofn.lpstrFileTitle[0] = '\0';
        ofn.nMaxFileTitle = sizeof(szFileTitle);
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetSaveFileNameA(&ofn))
        {
            if (strlen(ofn.lpstrFileTitle) < 4 || strcmp(ofn.lpstrFileTitle + strlen(ofn.lpstrFileTitle) - 4, ".txt") != 0)
            {

                strcat(ofn.lpstrFileTitle, ".txt");
                strcat(ofn.lpstrFile, ".txt");
            }
            if (currentFile = fopen(ofn.lpstrFile, "w+"))
            {
                isCreated = 1;
                fclose(currentFile);
            }
        }
        else
        {
            MessageBoxA(NULL, "创建失败", "TextEditor", MB_OK);
            isProcessFile = 0;
            return;
        }
    }

    currentFile = fopen(ofn.lpstrFile, "w+");
    PtrToLine p = headLine;
    while (p->nxtNode != NULL)
    {
        fprintf(currentFile, "%s\n", p->Text);
        p = p->nxtNode;
    }
    fprintf(currentFile, "%s", p->Text);
    isSaved = 1;
    isProcessFile = 0;
    fclose(currentFile);
}

void openFile()
{
    if (isProcessFile)
        return;
    isProcessFile = 1;

    int state;
    if (!isSaved)
    {
        if (!isCreated)
        {
            sprintf(msg, "是否要保存对 无标题 的更改？");
        }
        else
        {
            sprintf(msg, "是否要保存对 %s 的更改？", ofn.lpstrFileTitle);
        }
        state = MessageBoxA(NULL, msg, "TextEditor", MB_YESNOCANCEL | MB_ICONWARNING);
        switch (state)
        {
        case IDYES:
            saveFile();
            break;
        case IDNO:
            break;
        case IDCANCEL:
            isProcessFile = 0;
            return;
        }
    }

    char szFile2[512];
    char szFileTitle2[512];

    strcpy(szFile2, szFile);
    strcpy(szFileTitle2, szFileTitle);

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "文本文档(*.txt)\0*.txt\0所有文件(*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = szFileTitle;
    ofn.lpstrFileTitle[0] = '\0';
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn))
    {
        currentFile = fopen(ofn.lpstrFile, "r+");
        if (!currentFile)
        {
            MessageBoxA(NULL, "打开文件失败", "TextEditor", MB_OK | MB_TASKMODAL);
            strcpy(szFile, szFile2);
            strcpy(szFileTitle, szFileTitle2);
            isProcessFile = 0;
            return;
        }
    }
    else
    {
        MessageBoxA(NULL, "打开文件失败", "TextEditor", MB_OK);
        strcpy(szFile, szFile2);
        strcpy(szFileTitle, szFileTitle2);
        isProcessFile = 0;
        return;
    }

    /*	fseeko64(currentFile, 0, SEEK_END);
        _off64_t degree = ftello64(currentFile);
        fseeko64(currentFile, 0, SEEK_SET);
    */

    InitGUI();
    initModel();
    initEdit();

    PtrToLine p = headLine;
    char tmp[105], Cnt = 0;
    blockNode Cur = (blockNode){0, 0};
    while ((Cnt = fread(tmp, sizeof(char), 100, currentFile)) != 0)
    {
        Cur = inputContent(Cur, tmp, Cnt);
    }
    fclose(currentFile);
    setCursor(Cur);
    setStartSelect(Cur);
    setEndSelect(Cur);

    isSaved = 1;
    isCreated = 1;
    isProcessFile = 0;
}

void exitApplication()
{
    if (!isSaved)
    {
        if (!isCreated)
        {
            sprintf(msg, "是否要保存对 无标题 的更改？");
        }
        else
        {
            sprintf(msg, "是否要保存对 %s 的更改？", ofn.lpstrFileTitle);
        }
        int state = MessageBoxA(NULL, msg, "TextEditor", MB_YESNOCANCEL | MB_TASKMODAL);
        switch (state)
        {
        case IDYES:
            saveFile();
            ExitGraphics();
        case IDNO:
            ExitGraphics();
            break;
        case IDCANCEL:
            break;
        }
    }
    else
    {
        ExitGraphics();
    }
}
