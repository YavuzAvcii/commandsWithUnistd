#ifndef COMMANDS
#define COMMANDS

void showLogs(char cwd[]);
void createDir(char *dirName, char cwd[]);
void createFile(char *name, char cwd[]);
void listDir(char *dirName, char cwd[]);
void listFilesByExtension(char *dirName, char *extension, char cwd[]);
void listDirWrapper(char *folderName, char cwd[]);
void readFile(char *fileName, char cwd[]);
void appendToFile(char *fileName, char *text, char cwd[]);
void deleteFile(char *fileName, char cwd[]);
void deleteDir(char *folderName, char cwd[]);
void deleteWrapper(char *name, char cwd[], char type);
char *takeTime();
void commandLog(char *text, char cwd[]);

#endif
