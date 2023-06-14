#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>

#include "middleware.h"
#include "imgui.h"
#include "model.h"

extern PtrToLine headLine;
extern PtrToLine tailLine;

static FILE *currentFile; // 当前文件
OPENFILENAMEA ofn;        // utf-8 格式
static char szFile[512];  // 存储文件名
static char szFileTitle[512];
static int isSaved;           // 是否保存
static int isCreated;         // 是否被创建
static char msg[1010];        // 临时用
static int isProcessFile = 0; // 程序是否在读取/输出数据

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
    isSaved = 1;                   // 如果文本被修改，则替换为未保存
    isCreated = 0;                 // 暂未创建文件，文本最终要被写入到文件
    ZeroMemory(&ofn, sizeof(ofn)); // ofn用来存放文件对话框的各种信息（save和open两类对话框）
}

void saveFile()
{
    isProcessFile = 1;
    if (!isCreated)
    {
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile; // 这个哪里来
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
        { // 打开用于保存文件的对话框 ，对话框各部分信息在ofn里面
            if (strlen(ofn.lpstrFileTitle) < 4 || strcmp(ofn.lpstrFileTitle + strlen(ofn.lpstrFileTitle) - 4, ".txt") != 0)
            {
                // 文件名末尾是否有.txt，没有的话加上去 ，文件名长度小于4 或者文件名末4位与.txt不同（ofn.lpstrFileTitle就是自己写的文件名，字符串形式）
                strcat(ofn.lpstrFileTitle, ".txt");
                strcat(ofn.lpstrFile, ".txt");
            }
            if (currentFile = fopen(ofn.lpstrFile, "w+"))
            { // 打开该文件，成功打开的话，isCreated=1，已创建文件
                isCreated = 1;
                fclose(currentFile); // 成功创建了就没问题了，关了 ，下次saveFile再打开currentFile指针
            }
        }
        else
        {
            MessageBoxA(NULL, "文本文件未创建", "温馨提示", MB_OK);
            isProcessFile = 0;
            return;
        }
    }
    else
    {
        return;
    }

    currentFile = fopen(ofn.lpstrFile, "w+"); // 打开文件
    PtrToLine p = headLine;
    while (p != NULL)
    {
        fprintf(currentFile, "%s", p->Text); // 把链表里的内容全部读到文件里面，saveFile不会自动同步的
        p = p->nxtNode;
    }
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
            isProcessFile = 0; // 文件处理结束
            return;
        }
    }

    char szFile2[512]; // 文件名临时寄存点
    char szFileTitle2[512];

    strcpy(szFile2, szFile);
    strcpy(szFileTitle2, szFileTitle);

    ZeroMemory(&ofn, sizeof(ofn)); // 这一块内容相当于对文本对话框的相关信息进行赋值
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "文本文档(*.txt)\0*.txt\0所有文件(*.*)\0*.*\0"; // 这里代表文件的拓展名
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = szFileTitle;
    ofn.lpstrFileTitle[0] = '\0';
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn))
    {                                             // GetOpenFileName打开一个文本对话框，该对话框的信息由ofn提供
        currentFile = fopen(ofn.lpstrFile, "r+"); // 打开对话框中我们自己命名的文件，返还指向该文件的指针，用于把文本信息（缓冲区）写入文件（内存）
        if (!currentFile)
        { // 没打开 （比如文件名不存在的情况）
            MessageBoxA(NULL, "打开失败", "TextEditor", MB_OK | MB_TASKMODAL);
            strcpy(szFile, szFile2);
            strcpy(szFileTitle, szFileTitle2);
            isProcessFile = 0;
            return;
        }
    }
    else
    {
        MessageBoxA(NULL, "文件未打开", "温馨提示", MB_OK);
        strcpy(szFile, szFile2);
        strcpy(szFileTitle, szFileTitle2);
        isProcessFile = 0;
        return;
    }

    /*	fseeko64(currentFile, 0, SEEK_END);
        _off64_t degree = ftello64(currentFile);
        fseeko64(currentFile, 0, SEEK_SET);
    */
    // 这一段代码提供了文件的大小信息（内存或者说字节数），可能会用到

    InitGUI(); // 这个可有可无，功能与ZeroMemory相同

    setCursor((blockNode){0, 0}); // 光标位置的重置，与键盘和字符回调函数相关，在文件保存这里暂时不起作用
    setStartSelect((blockNode){0, 0});
    setEndSelect((blockNode){0, 0});

    // 将currentFile所指文件（我们选择打开的那个文件）的内容读入到链表里面 ，链表内容作为文本显示在屏幕上
    PtrToLine p = headLine; // 指针p用于遍历链表
    char tmp[105], Cnt = 0;
    blockNode Cur = (blockNode){0, 0};
    while ((Cnt = fread(tmp, sizeof(char), 100, currentFile)) != 0)
    {
        Cur = inputContent(Cur, tmp, Cnt);
    }
    // free(p);

    fclose(currentFile); // 已读完，关闭文件

    isSaved = 1;       // 相当于initFileConfig，重置保存状态，如果有进行编辑修改就置0
    isCreated = 1;     // 文件已经创建，下次保存的时候也是在同一个位置
    isProcessFile = 0; // 处理结束
}

void createFile()
{
    if (isProcessFile)
        return; // 正在处理文件（有对话框被打开时），拒绝二次相应
    if (!isSaved)
    { // 未保存（发生修改）
        if (!isCreated)
        { // 文件未被创建，文本信息（咱们的链表结构里面的Text）没有被存放进去
            sprintf(msg, "是否要保存对 无标题 的更改？");
        }
        else
        {
            sprintf(msg, "是否要保存对 %s 的更改？", ofn.lpstrFileTitle);
        }
        int state = MessageBoxA(NULL, msg, "TextEditor", MB_YESNOCANCEL | MB_TASKMODAL | MB_ICONWARNING); // Messagebox用于打开一个选择对话框（Yes or No）
        switch (state)
        {
        case IDYES:
            saveFile();
        case IDNO:
            initDisplay();
            break;
        case IDCANCEL:
            isProcessFile = 0;
            break;
        }
    }
    else
    {
        initDisplay();
    }
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
            ExitGraphics(); // 保存后退出
        case IDNO:
            ExitGraphics(); // 不保存退出
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
