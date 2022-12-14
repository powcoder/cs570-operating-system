https://powcoder.com
代写代考加微信 powcoder
Assignment Project Exam Help
Add WeChat powcoder
https://powcoder.com
代写代考加微信 powcoder
Assignment Project Exam Help
Add WeChat powcoder
#include <stdio.h>
#include "getword.h"
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>


/* Definitions that are used in p2.c */
#define EOF -255
#define ZERO_COMMANDS 0
#define FAIL -1
#define SUCCESS 0
#define STRING_EQUALS 0 
#define TRUE 1
#define FALSE 0
#define PARSE_ERROR -2

/* Constants in p2.h */
#define MAXITEM 100 /* max number of words */
#define MAXSIZE (STORAGE*MAXITEM) /*Max amount of characetr per commandline */


char *prompt = ":570: "; //shell prompt
int background_flag = 0; //flag for the & metachar
int pipe_flag = 0; // flag for the | meta char
int new_argv_size = 0; // holds the amount of arguments to be executed
int *outfile = NULL; //holds the name for outputfile
int *infile = NULL; //holds the name for the input file.
int redirection_flag = 0; //Flag for any type of redirection

extern int backslash_flag; // a global flag for getword.c and p2.c to see if a backslash occured
extern int background_flag_from_getword;// a global flag for getword.c and p2.c to see if a apersand is at the end of a line.

/*	
 * Parses the the input from stdin using getword to be organize into a char array to set
 * it up so it can be executed with an exevp.
 *
 * parameters:
 * 	commands: string array that holds the address fo the parsed commands.
 * 	line: hodls the the unparsed commadns from stdin from getword.c
 * return:
 * 	program returns the number of words that are in the command. This helps with 
 * 	piping later on.
 *
 */

int parse(char **commands, char *line);

/*
 * Executes the commands after a fork with the child process. 
 * parameters: command: a string array with the name of the executable.
 *	       args: a string array with the arugments or flags for the given for the command
 * Forks a child to execute the command which is handled in run_child_command.
 * Gives an eror to sterr if failed.
 * Also if it is a parent it waits on it's children to finish but if a background_flag is set
 * then it doesn't.
 *
 *
 */
void exec_command(char *command, char **args);

/*
 * Executes the command in the child forked from exec_command.
 * The method also calls the redirection
 * method called set_up_redirection if the redirection flag is set. Similarly if the pipe_flag is set
 * it forks another child and the grandchild reads/writes to it. Otherwise it just does an execvp
 * and executes the regualr command. 
 * input: command: a string with the name of the start of the executable
 * 	args: a string array wiht the argument/flags for the executable
 * output: The command is executed and it sent to stdout or a file.
 * exits if a error occurs on system call. 
 */

void run_child_command(char *command, char **args);
/*
 * Sets up input and out files using dup2() to uplicate the 
 * file descriptors of 0 and 1 or stdin and stdout respectively.
 * returns: an integer that is the file descriptor of the last file opened. 
 *
 */
int set__up_redirection();
/*
 * Handles the SIGTERM on program termination so that the autograder doesn't die.
 *
 */
void sighandler();

/*
 * Clears all the flags p2.c to FALSE
 *
 */
void clear_flags();

/*
 * Check for a file's existance by using access()
 * return:
 * 	  SUCCESS: if the file exists
 * 	  FAIL: if the file doesn't exist
 */
int file_exists(const char *file_name){
	return access(file_name, F_OK);
}
