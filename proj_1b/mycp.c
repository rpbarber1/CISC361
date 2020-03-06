#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

/* 
Ryan Barber
CISC361
Programming Assignment 2
*/


int main(int argc, char *argv[]) {

	int inputfd, outputfd; // input, output file descriptor

	//get file stats
	struct stat in_file;
	stat(argv[1], &in_file);
	int buffsize = 1024;
	char buf[buffsize];
	int nchar;
	
	//check if there are 3 arguments "cp <origin> <destination>"
	if(argc != 3){
		printf("usage: cp origin_name destination_name\n");
		exit(1);
	}
	//check if origin file does not open
	if((inputfd = open(argv[1], O_RDONLY)) < 0){
		fprintf(stderr, "can not open %s: ", argv[1]);
		perror("\0");
		exit(1);
	}
	//check if destination file exitst
	//if yes then ask for overwrite
	if((access(argv[2], F_OK)) == 0){
		printf("Do you want to overwrite %s (y/n): ", argv[2]);
		char option;
		scanf("%c", &option);
		switch(option){
			case 'y':
				outputfd = open(argv[2], O_WRONLY | O_TRUNC, in_file.st_mode);
				break;
			case 'n':
				printf("no overwrite, copy canceled\n");
				exit(1);
				break;
			default:
				printf("y/n not entered: copy canceled\n");
				exit(1);
		}
	} else {
		outputfd = open(argv[2], O_WRONLY | O_CREAT, in_file.st_mode);
	}
	
	//read input file
	while((nchar = read(inputfd, buf, buffsize)) > 0){
		//write to output file
		if((write(outputfd, buf, nchar)) != nchar){
			perror("write error");
			exit(1);
		}
	}
	//check for read error
	if(nchar == -1){
		perror("read error");
		exit(1);
	}
	//close files
	if((close(inputfd)) == -1 || (close(outputfd)) == -1){
		printf("error closing files");
		exit(1);	
	}

	return 0;
}
