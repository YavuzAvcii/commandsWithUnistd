#ifndef HELPERS
#define HELPERS
#define READ_FILE_BUFFER 1024
#define WRITE_BUFFER 128
#define CWD_BUFFER 128
#define LIST_BUFFER 1024
#define ERR_MSG_LEN 128
#define PATH_LEN 254

void logBasic(char *text);
void printHelper();
char *takeFullPath(char path[], char *name, char cwd[]);
int validateFolderName(char *folderName);

#endif
