#include "commands.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include "helpers.h"

void commandLog(char *text, char cwd[]);
char *takeTime();

void showLogs(char cwd[])
{
  readFile("log.txt", cwd);
}

void createDir(char *dirName, char cwd[]) {
  mode_t mode = 0777;
  char folderName[PATH_LEN];

  strcpy(folderName, cwd);
  strcat(folderName, "/");
  strcat(folderName, dirName);

  int status = mkdir(folderName, mode);
  if (status == 0)
  {
    char success[ERR_MSG_LEN] = "Directory ";
    strcat(success, dirName);
    strcat(success, " created successfully.");
    commandLog(success, cwd);
  }
  else
  {
    if (errno == EEXIST) 
    {
       char errMsg[ERR_MSG_LEN] = "ERROR: Directory ";
       strcat(errMsg, dirName);
       strcat(errMsg, " already exists.\n");
       logBasic(errMsg);
    }
    else 
    {
      char errMsg[ERR_MSG_LEN] = "createDir failed.\n";
      logBasic(errMsg);
    }
  }
}

void createFile(char *name, char cwd[])
{
  mode_t mode = 0777;
  char fileName[PATH_LEN];
  strcpy(fileName, cwd);
  strcat(fileName, "/");
  strcat(fileName, name);
  int fd = open(fileName, O_CREAT | O_EXCL | O_WRONLY, mode);
  if (fd == -1)
  {
    char errMsg[ERR_MSG_LEN] = "ERROR: the file ";
    strcat(errMsg, name);
    strcat(errMsg, " already exists.\n");
    logBasic(errMsg);
  }
  else
  {
    char *tm = takeTime();
    char writeBuffer[WRITE_BUFFER];
    memset(writeBuffer, 0, sizeof(writeBuffer));
    strcpy(writeBuffer, tm);
    strcat(writeBuffer, "\n");
    write(fd, writeBuffer, strlen(writeBuffer));
    char success[ERR_MSG_LEN] = "file ";
    strcat(success, name);
    strcat(success, " created successfully.");
    commandLog(success, cwd);
  }

  close(fd);
}


void listDir(char *dirName, char cwd[])
{
  char folderName[PATH_LEN];
  strcpy(folderName, cwd);
  strcat(folderName, "/");
  strcat(folderName, dirName);
  /* Check if folderName is valid */
  if(validateFolderName(folderName) != 0) 
  {
    return;
  }
  DIR *dir = opendir(folderName);

  if (!dir)
  {
    char errMsg[ERR_MSG_LEN] = "ERROR: Opening directory ";
    strcat(errMsg, folderName);
    strcat(errMsg, ".\n");
    logBasic(errMsg);
    return;
  }

  struct dirent *entry;
  while((entry = readdir(dir)) != NULL)
  {
    /* Print files unless they are current file or parent file */
    if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))
    {
      int len = strlen(entry->d_name);
      char dn[len + 1];
      strcpy(dn, entry->d_name);
      strcat(dn, "\n");
      logBasic(dn);
    }
  }
  char success[ERR_MSG_LEN] = "Listed all directories in ";
  strcat(success, folderName);
  strcat(success, " successfully.");
  commandLog(success, cwd);
  closedir(dir);
}

void listFilesByExtension(char *dirName, char *extension, char cwd[])
{
  char folderName[PATH_LEN];
  strcat(folderName, cwd);
  strcat(folderName, "/");
  strcat(folderName, dirName);

  if(validateFolderName(folderName) != 0)
  {
    return;
  }
  DIR *dir = opendir(folderName);

  if (!dir)
  {
    char errMsg[ERR_MSG_LEN] = "ERROR: Operning directory ";
    strcat(errMsg, folderName);
    strcat(errMsg, ".\n");
    logBasic(errMsg);
    return;
  }
  struct dirent *entry;
  while((entry = readdir(dir)) != NULL)
  {
    /* Print files unless they are current file or parent file */
    if (entry->d_type != DT_REG) continue;

    char *dot = strrchr(entry->d_name, '.');

    if (dot && (strcmp(dot, extension) == 0))
    {
      int len = strlen(entry->d_name);
      char dn[len + 1];
      strcpy(dn, entry->d_name);
      strcat(dn, "\n");
      logBasic(dn);
    }
  }
  char success[ERR_MSG_LEN] = "Listed all files with extension ";
  strcat(success, extension);
  strcat(success, " in ");
  strcat(success, folderName);
  strcat(success, " successfully.");
  commandLog(success, cwd);
  closedir(dir);
}

void listDirWrapper(char *folderName, char cwd[])
{
  pid_t pid= fork();
  if (pid < 0)
  {
    char *forkErr = "ERROR: couldn't fork\n";
    write(0, forkErr, sizeof(forkErr));
  }
  else if (pid == 0)
  {
    listDir(folderName, cwd);
  }
  else
  {
    waitpid(pid, NULL, 0);
  }
}

void listFilesByExtensionWrapper(char *folderName, char *extension, char cwd[])
{
  pid_t pid= fork();
  if (pid < 0)
  {
    char *forkErr = "ERROR: couldn't fork\n";
    write(0, forkErr, sizeof(forkErr));
  }
  else if (pid == 0)
  {
    listFilesByExtension(folderName, extension, cwd);
  }
  else
  {
    waitpid(pid, NULL, 0);
  }
}

void readFile(char *fileName, char cwd[])
{
  char fullPath[PATH_LEN];
  strcpy(fullPath, cwd);
  strcat(fullPath, "/");
  strcat(fullPath, fileName);

  int fd = open(fullPath, O_RDONLY);
  if (fd == -1)
  {
    char errMsg[ERR_MSG_LEN] = "ERROR: Couldn't open file\n";
    write(0, errMsg, sizeof(errMsg) * strlen(errMsg));
    return;
  }

  char buffer[READ_FILE_BUFFER];
  ssize_t bytesRead;

  while((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
  {
    write(0, buffer, bytesRead);
  }

  if (bytesRead == -1)
  {
    char errMsg[ERR_MSG_LEN] = "ERROR: Couldn't read file\n";
    write(0, errMsg, strlen(errMsg));
    close(fd);
    return;
  }
  if (strcmp(fileName, "log.txt") != 0)
  {
    char success[ERR_MSG_LEN] = "read file ";
    strcat(success, fileName);
    strcat(success, " successfully.");
    commandLog(success, cwd);
  }
  close(fd);
}

void appendToFile(char *fileName, char *text, char cwd[])
{
  char fullPath[PATH_LEN];
  strcpy(fullPath, cwd);
  strcat(fullPath, "/");
  strcat(fullPath, fileName);

  int fd = open(fullPath, O_WRONLY | O_APPEND);
  if (fd == -1)
  {
    char errMsg[ERR_MSG_LEN] = "ERROR: File ";
    strcat(errMsg, fileName);
    strcat(errMsg, " not found!\n");
    write(0, errMsg, strlen(errMsg));
  }
  else
  {
    char writeBuffer[WRITE_BUFFER];
    memset(writeBuffer, 0, sizeof(writeBuffer));
    strcpy(writeBuffer, text);
    strcat(writeBuffer, "\n");
    write(fd, writeBuffer, strlen(writeBuffer));
    if (strcmp(fileName, "log.txt") != 0)
    {
      char success[ERR_MSG_LEN] = "Added content to file ";
      strcat(success, fileName);
      strcat(success," successfully.");
      commandLog(success, cwd);
    }

  }
  close(fd);
}

void deleteFile(char *fileName, char cwd[])
{
  /* use unlink to delete a file */
  char fullPath[PATH_LEN];
  takeFullPath(fullPath, fileName, cwd);

  int status = unlink(fullPath);
  if (status == 0)
  {
    char successMsg[ERR_MSG_LEN] = "file ";
    strcat(successMsg, fileName);
    strcat(successMsg, " deleted successfully.");
    commandLog(successMsg, cwd);
    strcat(successMsg, "\n");
    logBasic(successMsg);
  }
  else 
  {
    char errMsg[ERR_MSG_LEN] = "ERROR: file ";
    strcat(errMsg, fileName);
    strcat(errMsg, " not found.\n");
    logBasic(errMsg);
  }
}

void deleteDir(char *folderName, char cwd[])
{
  char fullPath[PATH_LEN];
  takeFullPath(fullPath, folderName, cwd);

  int status = rmdir(fullPath);
  if (status == 0)
  {
    char successMsg[ERR_MSG_LEN] = "directory ";
    strcat(successMsg, folderName);
    strcat(successMsg, " deleted successfully.");
    logBasic(successMsg);
    commandLog(successMsg, cwd);
  }
  else
  {
    char errMsg[ERR_MSG_LEN] = "ERROR: directory ";
    strcat(errMsg, folderName);
    strcat(errMsg, " is not empty.\n");
    logBasic(errMsg);
  }
}

void deleteWrapper(char *name, char cwd[], char type)
{
  /* control delete operation by taking type of the delete f for file and d for directory */
  int p = fork();
  if (p == 0)
  {
    if (type == 'f')
    {
      deleteFile(name, cwd);
      return;
    }
    else if (type == 'd')
    {
      deleteDir(name, cwd);
    }
  }
  else
  {
    waitpid(p, NULL, 0);
  }
}

char *takeTime()
{
  time_t t = time(NULL);
  struct tm *tm = localtime(&t);
  char *time = asctime(tm);

  size_t len = strlen(time);
  if (len > 0 && time[len - 1] == '\n')
  {
    time[len - 1] = '\0';
  }

  return time;
}

void commandLog(char *text, char cwd[])
{
  char logMsg[ERR_MSG_LEN];
  char *tm = takeTime();
  strcat(logMsg, tm);
  strcat(logMsg, " ");
  strcat(logMsg, text);

  appendToFile("log.txt", logMsg, cwd);
}
