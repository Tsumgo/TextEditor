#ifndef __FILE_H__
#define __FILE_H__

void initFileConfig();

void createFile();

void openFile();

void saveFile();

void exitApplication();

int getFileProcessState();

int getSaveState();

void setSaveState(int newSaveState);

#endif