#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <glob.h>
#include "sh.h"
/*
	Ryan Barber: ID:702382750
	Humphrey Asare: ID: 702452289
	CISC 361 
	PROGRAMMING ASSIGNMENT 3: SIMPLE SHELL
*/

int sh( int argc, char **argv, char **envp )
{
	char *prompt = calloc(PROMPTMAX, sizeof(char));
	char *commandline = calloc(MAX_CANON, sizeof(char)); // whole commandline string
	/*
	command = first arg;
	arg = pointer used to hold token
	commandpath = pointer used to hold path string for which, where
	p = 
	pwd = working directory string
	owd = old working directory
	*/
	char *command, *arg, *commandpath, *p, *pwd, *owd;
	char **args = calloc(MAXARGS, sizeof(char*)); // array of command line strings
	//argsct = args count
	int uid, status, argsct,go = 1;
	struct passwd *password_entry;
	char *homedir;
	struct pathelement *pathlist;
	


	uid = getuid();
	password_entry = getpwuid(uid);  /* get passwd info */
	homedir = password_entry->pw_dir; /* Home directory to start out with*/
     
	if ( (pwd =getcwd(NULL, PATH_MAX+1)) == NULL ){
		perror("getcwd");
		exit(2);
	}
	owd = calloc(strlen(pwd) + 1, sizeof(char));
	memcpy(owd, pwd, strlen(pwd));
	prompt[0] = ' '; prompt[1] = '\0';


	pathlist = get_path(); // get linked list of path;
	
	
  	while (go) {
     	/* print your prompt */
		printf("%s [%s]>", prompt, pwd);

    /* get command line and process */ // WORKING
    	if( (p = fgets(commandline, MAX_CANON, stdin)) == NULL) {
		printf("^D\nUse \"exit\" to leave mysh.\n");
		continue;
    	}
	else{
		if(commandline[0] == '\n'){
			continue;
		}
        	if (commandline[strlen(commandline) - 1] == '\n'){
			/* replace newline with null */
			commandline[strlen(commandline) - 1] = 0;
        	}
		//arg holds the tokenized string from strtok
		arg = strtok(commandline, " ");
		int i = 0;
		int csource; // holds return value from glob call
		
		while(arg != NULL){
			glob_t paths; //create glob_t struct;
			csource = glob(arg, 0, NULL, &paths); //call glob
			if(csource == 0){ //glob finds match
				int j = 0;
				for(int j = 0; j < paths.gl_pathc ; j++){
					//malloc mem for args and assign values from glob
					args[i] = malloc(sizeof(char)*128);
					strcpy(args[i],paths.gl_pathv[j]);
					i++;
				}
			}
			 else if(csource == GLOB_NOMATCH){ //glob doesn't find match
				//malloc mem for args and assign args[i] to arg
				args[i] = malloc(sizeof(char)*128);
				strcpy(args[i], arg);
				i++;
			}
			globfree(&paths); //free glob struct data
			arg = strtok(NULL, " ");
		}
		args[i] = NULL;//set last arg to null pointer
		argsct = i-1; //number of args after command
		command = args[0];
		
		
	}
		
    /* check for each built in command and implement 
	each block compares the command to one of the
	built in functions. If it's a match, the inner
	if() statements check the number of arguments
	and calls the correct function with the 
	correct arguments or prints an error message. */
   	if(strcmp(command, "exit") == 0){
		printf("Executing built-in [%s]\n", command);
        	break;//break loop, call exit after freeing mem
	}
	else if(strcmp(command, "which") == 0){
		if(argsct == 0){
			printf("Usage:\n which \"filename\"\n");
		}
		else{
			printf("Executing built-in [%s]\n", command);
			for(int i = 1; i < argsct+1; i++){
				which(args[i], pathlist);
			}
		}
	}
	else if(strcmp(command, "where") == 0){
		if(argsct == 0){
			printf("Usage:\n where \"filename\"\n");
		}
		else{
			printf("Executing built-in [%s]\n", command);
			for(int i = 1; i < argsct+1; i++){
				where(args[i], pathlist);
			}
		}
	}
	else if(strcmp(command, "cd") == 0){
		if(argsct == 0){
			printf("Executing built-in [%s]\n", command);
			changeDir(homedir, owd, pwd);
		}
		else if(argsct == 1){
			printf("Executing built-in [%s]\n", command);
			changeDir(args[1], owd, pwd);
		}
		else{
			printf("Too many arguments.\n");
		}

	}
	else if(strcmp(command, "pwd") == 0){
		if(argsct > 0){
			printf("Too many arguments.\n");
		}
		else{
			printf("Executing built-in [%s]\n", command);
			printWorkingDir(pwd);
		}
	}
	else if(strcmp(command, "list") == 0){
		if(argsct == 0){
			printf("Executing built-in [%s]\n", command);
			list(pwd);
		}
		else {
			printf("Executing built-in [%s]\n", command);
			for(int i = 1; i < argsct+1; i++){
				list(args[i]);
			}

		}
	}
	else if(strcmp(command, "pid") == 0){
		if(argsct > 0){
			printf("Too many arguments.\n");
		} else {
			printf("Executing built-in [%s]\n", command);
			printID();
		}
	}
	else if(strcmp(command, "kill") ==0){
		if(argsct == 0 || argsct > 2){
			printf("Usage:\nkill [p | pid]\nkill [-s | signal num] [p | pid]\n");
		}
		else if(argsct == 1) {
			printf("Executing built-in [%s]\n", command);
			int tmpPid = atoi(args[1]);
			killProc(SIGTERM, tmpPid);
		}
		else if(argsct == 2) {	
			printf("Executing built-in [%s]\n", command);
			int tmpPid = atoi(args[2]);
			int tmpSig = atoi(&args[1][1]);
			killProc(tmpSig, tmpPid);
		}
	}
	else if(strcmp(command, "prompt") == 0){
		if(argsct == 0){
			printf("Executing built-in [%s]\n", command);
			printf("Enter prompt prefix: ");
			char *newPrompt = calloc(PROMPTMAX,sizeof(char));
			if(fgets(newPrompt, PROMPTMAX, stdin) == NULL){
				perror("Error");
			}
			newPrompt[strlen(newPrompt)-1] = 0;
			changePrompt(prompt, newPrompt);
			free(newPrompt);
		}
		else if(argsct == 1){
			printf("Executing built-in [%s]\n", command);
			changePrompt(prompt, args[1]);
		}
		else {
			printf("Too many arguments.\n");
		}
	}
	else if(strcmp(command, "printenv") == 0) {
		if(argsct == 0){
			printf("Executing built-in [%s]\n", command);
			printenv(envp);
		}
		else if(argsct == 1){
			printf("Executing built-in [%s]\n", command);
			if(getenv(args[1]) != NULL){
				printf("%s\n",getenv(args[1]));
			}
		}
		else{
			printf("Too many arguments.\n");
		}
	}
	else if(strcmp(command, "setenv") == 0){
		if(argsct == 0){
			printf("Executing built-in [%s]\n", command);
			printenv(envp);
		}
		else if(argsct == 1){
			printf("Executing built-in [%s]\n", command);
			char *tmpValue = "";
			setEnvironment(args[1], tmpValue);
			envp = __environ;
			if(strcmp(args[1], "PATH") == 0){
				struct pathelement *temp;
				while(pathlist != NULL){//walk the list
					temp = pathlist;
					pathlist = pathlist->next;
					free(temp);
				} 
				
				pathlist = get_path();
			}
			else if(strcmp(args[1], "HOME") == 0){
				strcpy(homedir, getenv("HOME"));
			}
		}
		else if(argsct == 2){
			printf("Executing built-in [%s]\n", command);
			setEnvironment(args[1], args[2]);
			envp = __environ;
			if(strcmp(args[1], "PATH") == 0){
				struct pathelement *temp;
				while(pathlist != NULL){//walk the list
					temp = pathlist;
					pathlist = pathlist->next;
					free(temp);
				} 
				
				pathlist = get_path();
			}
			else if(strcmp(args[1], "HOME") == 0){
				strcpy(homedir, getenv("HOME"));
			}
		}
		else{
			printf("Too many arguments.\n");
		}
	}
	/*  else  program to exec 
	 find it do fork(), execve() and waitpid() 
         else { fprintf(stderr, "%s: Command not found.\n", args[0]); } */
	else{
		if ((pid = fork()) < 0) { //error
			perror("Error");
			exit(1);
		}
		else if (pid == 0) { //child
			execvp(args[0],args);
			//if execvp returs there's an error
			perror("Error");
			exit(127);
		}
		else { //parent
			pid = wait(NULL);
		}	
	}


	//clear **args for next command
	int i = 0;
	while(i < argsct+1){
		free(args[i]);
		i++;
	}
  }//end of while loop
	//free memory
	free(pwd);
	free(prompt);
	free(owd);
	free(commandline);
	free(command);

	struct pathelement *temp;
	while(pathlist ){
		temp = pathlist;
		pathlist = pathlist->next;
		free(temp);
		
	}
	free(pathlist);
  	free(args);
	exitShell();//exit shell here to free mem first
  return 0;
} /* sh() */

void exitShell(){
	//Print exit messgae and exit shell
	printf("Exiting shell\n");
	exit(0);
} // end exit

void which(char *command, struct pathelement *pathlist ) {
	/* loop through pathlist until finding command and return it.
	Return NULL when not found. */
	printf("%s:\n", command);
	char found = 'f';
	char *filepath = malloc(sizeof(char)*512);
	while(pathlist){
		//filepath = concatenation of pathlist element and command
		sprintf(filepath,"%s/%s", pathlist->element, command);
		if(access(filepath, F_OK & X_OK) == 0){
			found = 't';
			printf("%s\n", filepath);
			break;
		}
		pathlist = pathlist->next;
	}
	if(found == 'f'){
		printf("Command not found in path\n");
	}
	free(filepath);
} /* which() */ 

void where(char *command, struct pathelement *pathlist ){
  /* similarly loop through finding all locations of command */

	printf("%s:\n", command);
	char *filepath = malloc(sizeof(char)*512);
	char found = 'f'; //boolean
	while(pathlist){
		//filepath = concatenation of pathlist element and command
		sprintf(filepath,"%s/%s", pathlist->element, command);
		if(access(filepath, F_OK & X_OK) == 0){
			printf("%s\n",filepath);
			found = 't';
		}
		pathlist = pathlist->next;
	}
	if(found == 'f'){
		printf("Command not found in path\n");
	}
	free(filepath);

} /* where() */

void changeDir(char *newDir, char *owd, char *pwd){
	// arg '-' goes to previous dir
	// arg 'path' goes to that path

	char *tmpdir = malloc(sizeof(char)*512);
	if(strcmp(newDir, "-") == 0){
		strcpy(tmpdir, owd);// tmpdir is previous
		getcwd(owd, sizeof(owd)); //set owd
		if(chdir(tmpdir) == -1){ // set current to original prevwd
			strcpy(owd,tmpdir);//undo set owd
			perror("Error");
			return;
		}
		getcwd(pwd,PATH_MAX+1);
	}
	else{
		strcpy(tmpdir, owd);
		getcwd(owd, sizeof(owd)); //set owd
		if(chdir(newDir) == -1){ //change to home
			strcpy(owd,tmpdir); //undo change
			perror("Error");
			return;
		}
		getcwd(pwd,PATH_MAX+1);
	}
	free(tmpdir);
	
} // end of changeDir

void printWorkingDir(char *pwd){
	printf("%s\n", pwd);
} //end of pwd


void list( char *dir ){
  /* see man page for opendir() and readdir() and print out filenames for
  the directory passed */
	struct dirent *tmpdirent;
	DIR *tmpdir; //directory stream pointer
	if((tmpdir = opendir(dir)) == NULL){ //open directory
		perror("Error");
		return;
	}

	printf("\n%s:\n", dir);
	while((tmpdirent = readdir(tmpdir)) != NULL){ //read directory
		printf("\t%s\n", tmpdirent->d_name);
	}
	free(tmpdir);
	free(tmpdirent);
} /* list() */

void printID(){
	printf("shell pid: %d\n", getpid());
} //end of pid

void killProc(int sig, int pid){
	if((kill(pid, sig)) == -1){
		perror("Error");
	}
} //end of kill

void changePrompt(char *prompt, char *newPrompt){
	strcpy(prompt, newPrompt);
	prompt[strlen(prompt)] = '\0';
} //end prompt

void printenv(char **envp){
	for(int i = 0; envp[i] != NULL; i++){
		printf("%s\n", envp[i]);
	}
} // end printenv

void setEnvironment(char *name, char *value){
	//use setenv library function
	if(setenv(name, value, 1) == -1){
		perror("Error");
	}
	
}//end setenv

