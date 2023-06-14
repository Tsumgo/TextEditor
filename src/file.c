#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>

#include "middleware.h"
#include "imgui.h"
#include "model.h"

extern PtrToLine headLine;
extern PtrToLine tailLine;

static FILE *currentFile; // ��ǰ�ļ�
OPENFILENAMEA ofn;        // utf-8 ��ʽ
static char szFile[512];  // �洢�ļ���
static char szFileTitle[512];
static int isSaved;           // �Ƿ񱣴�
static int isCreated;         // �Ƿ񱻴���
static char msg[1010];        // ��ʱ��
static int isProcessFile = 0; // �����Ƿ��ڶ�ȡ/�������

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
    isSaved = 1;                   // ����ı����޸ģ����滻Ϊδ����
    isCreated = 0;                 // ��δ�����ļ����ı�����Ҫ��д�뵽�ļ�
    ZeroMemory(&ofn, sizeof(ofn)); // ofn��������ļ��Ի���ĸ�����Ϣ��save��open����Ի���
}

void saveFile()
{
    isProcessFile = 1;
    if (!isCreated)
    {
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile; // ���������
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "�ı��ĵ�(*.txt)\0*.txt\0�����ļ�(*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = szFileTitle;
        ofn.lpstrFileTitle[0] = '\0';
        ofn.nMaxFileTitle = sizeof(szFileTitle);
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetSaveFileNameA(&ofn))
        { // �����ڱ����ļ��ĶԻ��� ���Ի����������Ϣ��ofn����
            if (strlen(ofn.lpstrFileTitle) < 4 || strcmp(ofn.lpstrFileTitle + strlen(ofn.lpstrFileTitle) - 4, ".txt") != 0)
            {
                // �ļ���ĩβ�Ƿ���.txt��û�еĻ�����ȥ ���ļ�������С��4 �����ļ���ĩ4λ��.txt��ͬ��ofn.lpstrFileTitle�����Լ�д���ļ������ַ�����ʽ��
                strcat(ofn.lpstrFileTitle, ".txt");
                strcat(ofn.lpstrFile, ".txt");
            }
            if (currentFile = fopen(ofn.lpstrFile, "w+"))
            { // �򿪸��ļ����ɹ��򿪵Ļ���isCreated=1���Ѵ����ļ�
                isCreated = 1;
                fclose(currentFile); // �ɹ������˾�û�����ˣ����� ���´�saveFile�ٴ�currentFileָ��
            }
        }
        else
        {
            MessageBoxA(NULL, "�ı��ļ�δ����", "��ܰ��ʾ", MB_OK);
            isProcessFile = 0;
            return;
        }
    }
    else
    {
        return;
    }

    currentFile = fopen(ofn.lpstrFile, "w+"); // ���ļ�
    PtrToLine p = headLine;
    while (p != NULL)
    {
        fprintf(currentFile, "%s", p->Text); // �������������ȫ�������ļ����棬saveFile�����Զ�ͬ����
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
            sprintf(msg, "�Ƿ�Ҫ����� �ޱ��� �ĸ��ģ�");
        }
        else
        {
            sprintf(msg, "�Ƿ�Ҫ����� %s �ĸ��ģ�", ofn.lpstrFileTitle);
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
            isProcessFile = 0; // �ļ��������
            return;
        }
    }

    char szFile2[512]; // �ļ�����ʱ�Ĵ��
    char szFileTitle2[512];

    strcpy(szFile2, szFile);
    strcpy(szFileTitle2, szFileTitle);

    ZeroMemory(&ofn, sizeof(ofn)); // ��һ�������൱�ڶ��ı��Ի���������Ϣ���и�ֵ
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "�ı��ĵ�(*.txt)\0*.txt\0�����ļ�(*.*)\0*.*\0"; // ��������ļ�����չ��
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = szFileTitle;
    ofn.lpstrFileTitle[0] = '\0';
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn))
    {                                             // GetOpenFileName��һ���ı��Ի��򣬸öԻ������Ϣ��ofn�ṩ
        currentFile = fopen(ofn.lpstrFile, "r+"); // �򿪶Ի����������Լ��������ļ�������ָ����ļ���ָ�룬���ڰ��ı���Ϣ����������д���ļ����ڴ棩
        if (!currentFile)
        { // û�� �������ļ��������ڵ������
            MessageBoxA(NULL, "��ʧ��", "TextEditor", MB_OK | MB_TASKMODAL);
            strcpy(szFile, szFile2);
            strcpy(szFileTitle, szFileTitle2);
            isProcessFile = 0;
            return;
        }
    }
    else
    {
        MessageBoxA(NULL, "�ļ�δ��", "��ܰ��ʾ", MB_OK);
        strcpy(szFile, szFile2);
        strcpy(szFileTitle, szFileTitle2);
        isProcessFile = 0;
        return;
    }

    /*	fseeko64(currentFile, 0, SEEK_END);
        _off64_t degree = ftello64(currentFile);
        fseeko64(currentFile, 0, SEEK_SET);
    */
    // ��һ�δ����ṩ���ļ��Ĵ�С��Ϣ���ڴ����˵�ֽ����������ܻ��õ�

    InitGUI(); // ������п��ޣ�������ZeroMemory��ͬ

    setCursor((blockNode){0, 0}); // ���λ�õ����ã�����̺��ַ��ص�������أ����ļ�����������ʱ��������
    setStartSelect((blockNode){0, 0});
    setEndSelect((blockNode){0, 0});

    // ��currentFile��ָ�ļ�������ѡ��򿪵��Ǹ��ļ��������ݶ��뵽�������� ������������Ϊ�ı���ʾ����Ļ��
    PtrToLine p = headLine; // ָ��p���ڱ�������
    char tmp[105], Cnt = 0;
    blockNode Cur = (blockNode){0, 0};
    while ((Cnt = fread(tmp, sizeof(char), 100, currentFile)) != 0)
    {
        Cur = inputContent(Cur, tmp, Cnt);
    }
    // free(p);

    fclose(currentFile); // �Ѷ��꣬�ر��ļ�

    isSaved = 1;       // �൱��initFileConfig�����ñ���״̬������н��б༭�޸ľ���0
    isCreated = 1;     // �ļ��Ѿ��������´α����ʱ��Ҳ����ͬһ��λ��
    isProcessFile = 0; // �������
}

void createFile()
{
    if (isProcessFile)
        return; // ���ڴ����ļ����жԻ��򱻴�ʱ�����ܾ�������Ӧ
    if (!isSaved)
    { // δ���棨�����޸ģ�
        if (!isCreated)
        { // �ļ�δ���������ı���Ϣ�����ǵ�����ṹ�����Text��û�б���Ž�ȥ
            sprintf(msg, "�Ƿ�Ҫ����� �ޱ��� �ĸ��ģ�");
        }
        else
        {
            sprintf(msg, "�Ƿ�Ҫ����� %s �ĸ��ģ�", ofn.lpstrFileTitle);
        }
        int state = MessageBoxA(NULL, msg, "TextEditor", MB_YESNOCANCEL | MB_TASKMODAL | MB_ICONWARNING); // Messagebox���ڴ�һ��ѡ��Ի���Yes or No��
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
            sprintf(msg, "�Ƿ�Ҫ����� �ޱ��� �ĸ��ģ�");
        }
        else
        {
            sprintf(msg, "�Ƿ�Ҫ����� %s �ĸ��ģ�", ofn.lpstrFileTitle);
        }
        int state = MessageBoxA(NULL, msg, "TextEditor", MB_YESNOCANCEL | MB_TASKMODAL);
        switch (state)
        {
        case IDYES:
            saveFile();
            ExitGraphics(); // ������˳�
        case IDNO:
            ExitGraphics(); // �������˳�
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
