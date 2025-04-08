#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include "helpers.h"

void logBasic(char *text)
{
  int len = strlen(text);
  char toWrite[len + 1];
  strcpy(toWrite, text);
  write(0, toWrite, sizeof(*toWrite) * (len + 1));
}

void printHelper()
{
  logBasic("Usage: fileManager <command> [arguments]\n\n");
  logBasic("Commands:\n");
  logBasic("createDir \"folderName\"\t\t\t -Create new directory\n");
  logBasic("createFile \"fileName\"\t\t\t -Create new file\n");
  logBasic("listDir \"folderName\"\t\t\t -List all files in a directory\n");
  logBasic("listFilesByExtension \"folderName\" \".txt\" -List files with a specific extension\n");
  logBasic("readFile \"fileName\"\t\t\t -Read a file's content\n");
  logBasic("appendToFile \"fileName\" \"new content\"\t -Append content to a file\n");
  logBasic("deleteFile \"fileName\"\t\t\t -Delete a file\n");
  logBasic("deleteDir \"folderName\"\t\t\t -Delete an empty folder\n");
  logBasic("showLogs \t\t\t\t -Display operation logs\n");
  // Continue updating here everytime you add a new command function
}

char *takeFullPath(char path[], char *name, char cwd[])
{
  strcpy(path, cwd);
  strcat(path, "/");
  strcat(path, name);
  return path;
}

/* return 0 if folderName is okay and 1 if it is not*/
int validateFolderName(char *folderName)
{
  struct stat path_stat;
  if(stat(folderName, &path_stat) != 0)
  {
    char cannotAccess[ERR_MSG_LEN] = "ERROR: Cannot access ";
    strcat(cannotAccess, folderName);
    strcat(cannotAccess, ".\n");
    logBasic(cannotAccess);
    return 1;
  }
  if (!S_ISDIR(path_stat.st_mode))
  {
    logBasic("ERROR: Path does not exits.\n");
    return 1;
  }

  return 0;
}
