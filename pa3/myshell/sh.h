
#include "get_path.h"

int pid;
int sh( int argc, char **argv, char **envp);
void exitShell();
void which(char *command, struct pathelement *pathlist);
void where(char *command, struct pathelement *pathlist);
void changeDir(char *newDir, char *owd, char *pwd);
void printWorkingDir(char *pwd);
void list ( char *dir );
void printID();
void killProc(int sig, int pid);
void changePrompt(char *prompt, char *newPrompt);
void printenv(char **envp);
void setEnvironment(char *name, char *value);

#define PROMPTMAX 32
#define MAXARGS 10
