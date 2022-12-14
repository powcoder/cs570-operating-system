https://powcoder.com
代写代考加微信 powcoder
Assignment Project Exam Help
Add WeChat powcoder
https://powcoder.com
代写代考加微信 powcoder
Assignment Project Exam Help
Add WeChat powcoder
#include <stdio.h> //perror, fflush, getline
#include "getword.h"
#include <fcntl.h> //open, access
#include <signal.h> //sigaction, signal, killpg
#include <sys/stat.h> //open, stat
#include <unistd.h> //dup2,execvp,chdir,fork, access, pipe, setpgid, getpgrp, getcwd, stat
#include <sys/types.h> //fork, open, wait, setpgid, stat
#include <sys/wait.h> //wait
#include <stdlib.h>//getenv,setenv,exit 
#include <linux/limits.h>
#include <pwd.h>
#include <libgen.h> //basename
#include <string.h> //strdup, strtok

/*Errors codes */
#define NESTED_PIPELINE_PIPE_FAILED         -34
#define PIPE_INPUT_REDIRECT_FAILED          -33
#define PIPE_OUTPUT_REDIRECT_FAILED         -32
#define FORK_FAILED_EXIT_CODE               -31
#define EXEC_FAILED_EXIT_CODE               -30
#define PIPE_FAILED_EXIT_CODE               -29
#define CD_INVALID_NUM_ARGS                 -28
#define CD_FAIL                             -27
#define DIRECTORY_OPEN_ERROR                -20
#define INPUT_FILE_OPEN_ERROR               -19
#define INPUT_REDIRECTION_FAILED            -18
#define OUTPUT_REDIRECTION_FAILED           -17
#define OUTPUT_FILE_OPEN_ERROR              -16
#define REDIRECTION_FILE_EXISTS             -15
#define REDIRECTION_ERROR                   -14
#define AMBIGUOUS_INPUT_REDIRECTION         -13
#define AMBIGUOUS_OUTPUT_REDIRECTION        -12
#define MULTIPLE_PIPELINES                  -11
#define NO_PROCESS_AFTER_PIPE               -10
#define PIPE_FORMAT_ERROR                    -9
#define BACKGROUND_FORMAT_ERROR              -8
#define OUTPUT_REDIRC_FORMAT_ERROR           -7
#define INPUT_REDIRC_FORMAT_ERROR            -6
#define NO_EXECUTABLE_AFTER_INPUT_REDIRC     -5
#define NO_EXECUTABLE_AFTER_OUTPUT_REDIRC    -4
#define DUP_FAILED			     -3
#define PARSE_ERROR                          -2


/* Definitions that are used in p2.c */
#define EOF -255
#define OK_TO_EXECUTE_COMMANDS 1 //Used to allow signal the ok to execute commands
#define ZERO_COMMANDS 0
#define FAIL -1
#define SUCCESS 0
#define STRING_EQUALS 0 
#define TRUE 1
#define FALSE 0
#define PARSE_ERROR -2

/* Constants in p2.h */
#define MAXITEM 100 /* max number of words */
#define MAXSIZE (STORAGE*MAXITEM) /*Max amount of character per commandline */
#define MAX_PIPES 10 //Maximum amount of pipe for p4

char *prompt = ":570: "; //shell prompt
char cwd[PATH_MAX]; //Path_MAX from <limits.h> . Will use for getcwd()
int prompt_changed_flag = FALSE; //Set when using cd 
int slash_special_case_flag = FALSE; //use when environ HOME /
char *env_variable; //Stores environment variable
int background_flag = 0; //flag for the & metachar
int pipe_flag = 0; // flag for the | meta char
int hereis_flag = 0; //flag for << meta character
int stop_reading_commands_flag = FALSE; //Comamnds to stop reading from stdin. Used for hereis << command
int dont_execute_flag = FALSE; //Flag for bad command that still executes. Used for bad input and hereis redirect commands
int username_lookup_fail_flag =FALSE; //flag to not fork a child if there is a bad username look up.
int environment_var_fail_flag = FALSE; //flag to not fork a child if there is a nonexistant environment var
int new_argv_size = 0; // holds the amount of arguments to be executed
int *outfile = NULL; //holds the name for outputfile
int *infile = NULL; //holds the name for the input file.
char *tmp_name = "TMPFILE"; //Global filename for new_argv to access
char *hereis_delimiter; //Holds the address to the delimiter document/word
FILE *hereis_doc = NULL; //Holds address to file here the contents of hereis is held.
int file_descriptor_in; //file descriptor for hereis operation.
//int *file_descriptor_ptr; //Pointer for file desriptor for a file to be used instead of stdin.
int saved_stdin; //Use to restore stdin after hereis << command 
int redirection_flag = 0; //Flag for any type of redirection
int pipe_location[MAX_PIPES];// locations of the pipes in the array of commands
int fds[MAX_PIPES*2]; //holds file descriptors for pipes
int fds[MAX_PIPES*2]; //holds file descriptors for pipes
extern int backslash_flag; // a global flag for getword.c and p2.c to see if a backslash occured
extern int background_flag_from_getword;// a global flag for getword.c and p2.c to see if a apersand is at the end of a line.
extern int tilde_flag;

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
int set_up_redirection();
/*
 * Handles the SIGTERM on program termination so that the autograder doesn't die.
 *
 */

void sighandler();

/*
 * Sets up the shell to take input from stdin continue
 * continue until it reaches the said deliminter document/word.
 */
int set_up_hereis_doc();

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
/*	
 * Executes up to 10 pipes with vertical piping. Each child will create a grandchil
 * to run a command. The last child will execute the first command and break out
 * of the the process and reutrn the function run_child_command() and 
 * continute executing the program normaly.
 * input: command: a string with the name of the executable
 * 	  args: a string array with the arguments/flags for the given executable
 * output:the result fo the execution on stdout for the file to which it was redirected
 * Error: Outputs appropriate error for 
 * 	  Nested pipes, Fork failed, pipe output/input redirect,
 *	  exec failed, and forked failed.  
 */
void nested_pipeline(char* command, char** args);

/*
 * Converts an int into a string by using 
 * sprintf() with a %d falg.
 * Used for debugging pipes.
 *
 */

void to_string(int number, char* string){
	sprintf(string, "%d",number);
}
/*
 * Used for the << metacharacter to remove a string
 * on a line which is the delimiter.
 */
int search_in_File(char *fname, char *str);
/*
 *  Appends a char to the end of a string.
 */
void append(char*, char c);
/*
 * Prints out the corresponding errors int the shell and calls clearflags
 * at the very end
 */
void print_error(int error_code, char* arg);
