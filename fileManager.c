#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

/*MACROS*/
#define ERR_MSG_LEN 128

/* custom headers */
#include "helpers.h"
#include "commands.h"

int main(int argc, char *argv[])
{
  int fd = open("log.txt", O_WRONLY | O_APPEND);
  if (fd == -1)
  {
    fd = open("log.txt", O_CREAT | O_WRONLY | O_APPEND, 0777);
  }

  char cwd[CWD_BUFFER];
  if (getcwd(cwd, sizeof(cwd)) == NULL)
  {
    logBasic("Couldn't reach the current working directory.\n");
    return 1;
  }
  if (argc == 1)
  {
    printHelper();
  }
  else if (strcmp(argv[1], "createDir") == 0 && argc == 3)
  {
    createDir(argv[2], cwd);
  }
  else if (strcmp(argv[1], "createFile") == 0 && argc == 3)
  {
    createFile(argv[2], cwd);
  }
  else if (strcmp(argv[1], "listDir") == 0 && argc == 3)
  {
    listDirWrapper(argv[2], cwd);
  }
  else if (strcmp(argv[1], "listFilesByExtension") == 0 && argc == 4)
  {
    listFilesByExtension(argv[2], argv[3], cwd);
  }
  else if (strcmp(argv[1], "readFile") == 0 && argc == 3)
  {
    readFile(argv[2], cwd);
  }
  else if (strcmp(argv[1], "appendToFile") == 0 && argc == 4)
  {
    appendToFile(argv[2], argv[3], cwd);
  }
  else if (strcmp(argv[1], "deleteFile") == 0 && argc == 3)
  {
    deleteWrapper(argv[2], cwd, 'f');
  }
  else if (strcmp(argv[1], "deleteDir") == 0 && argc == 3)
  {
    deleteWrapper(argv[2], cwd, 'd');
  }
  else if (strcmp(argv[1], "showLogs") == 0 && argc == 2)
  {
    showLogs(cwd);
  }
  else
  {
    char errMsg[ERR_MSG_LEN] = "ERROR: something is wrong please follow this: ";
    logBasic(errMsg);
    printHelper();
  }
  close(fd);
  return 0;
}
