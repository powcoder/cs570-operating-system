https://powcoder.com
代写代考加微信 powcoder
Assignment Project Exam Help
Add WeChat powcoder
https://powcoder.com
代写代考加微信 powcoder
Assignment Project Exam Help
Add WeChat powcoder
#include "p2.h"

/*
 * Class Info - Program #2 Bernardo Cervantes, John Carroll, CS570 program 2
 * 		Due : 10/5/18 for extra credit and 10/10/18 for regular deadline
 * Synopsis   - Program acts as a simple unix shell. It reads from stdin and parses
 * 		the line and returns the corresponding command output. It handles 
 * 		the metacharacters {'&','<','>','|','\' and EOF. Only hands single pipes
 * 		for input & output redirection.
 * 			- Program uses getword.c to get STDIN and parse it into a string array.
 * 			- Array is a parsed and then passed to evecvp to execute the commands.
 * 			- To handle execution we use system calls like dup2, execvp, fork, and pipe, etc.
 *
 */
char *new_argv[MAXITEM];// Holds addressed to parsed commands to be executed by the shell.
char line[MAXSIZE]; // Holds the unparsed line from stidn
int main(){
	//int exit_code;
	setpgid(0,0); // Sets current process and children to its own process group
	(void) signal(SIGTERM,sighandler); // Signal handler to catch SIGTERM
	while(TRUE){
		printf("%s", prompt);
		new_argv_size = parse(new_argv, line);

		if(new_argv_size == EOF){
			break;

		}
		else if(new_argv_size == ZERO_COMMANDS){
			fflush(stdout);
			continue;
		}

		else if(new_argv_size < PARSE_ERROR){
			fflush(stdout);
			perror("Parse error");
			fflush(stderr);
			continue;	
		}
		/* Handing cd for the shell since you cannot
		 * use execvp to execute cd.
		 *
		 */
		else if(strcmp(new_argv[0], "cd") == STRING_EQUALS){
			if(new_argv_size == 1){

				if(chdir(getenv("HOME")) == FAIL){
					perror("cd command failed to go HOME");	
				}
				else{
					clear_flags();
				}
			}

			/* Case for two arguments to change directory*/
			else if(new_argv_size == 2){
				if(chdir(new_argv[1]) == FAIL){
					perror("Failed to cd into directory");		
				}
				else{
					clear_flags();
				}	
			}
			else {
				perror("cd has an invalid number of arguments");
			}



		}


		else {
			exec_command(new_argv[0], new_argv);
			clear_flags();


		}


	}

	killpg(getpgrp(),SIGTERM);
	printf("p2 terminated.\n");
	return 0;
}

/* Parameters:Takes a pointer to a char pointer array which is new_argv, 
 * and pointer to a char array line which is line
 */

int parse(char **commands , char *line){ 
	int word_count = 0;
	int word_size = 0;
	int output_redirection = FALSE;
	int input_redirection = FALSE;

	do {
		if(word_size = getword(line)) { 
			if(strcmp(line, "&") == STRING_EQUALS && backslash_flag == FALSE && background_flag_from_getword == TRUE){
				background_flag = TRUE;
				break;
			}


			else if (strcmp(line, "|") == STRING_EQUALS) {
				/* if statement executes if pipe_flag is anything but 0. 
				 * Case preventing more than one pipe which will be handled in p4.c according to class lecture.	
				 */
				if (pipe_flag) { 
					perror("Cannot have multiple pipes");		
					return -1;
				}
				if (word_count == ZERO_COMMANDS) { // case where pipe is the only arguement on line/STDIN 
					perror("Need to an argument to pipe to.");
					return -1;
				} else {
					pipe_flag = word_count; // setting pipe_flag to address of pipeline char
				}
				*commands++ = NULL; // set the argument at the pipeflag to be null to split the 1st process from
				// the second.a

			}
			else if (strcmp(line, ">") == STRING_EQUALS) { // if the symbol is '>' set up output redirection
				if (output_redirection) { // if more than one '>' symbol is found set error flag to clear the stream.
					perror("Ambiguous redirection output. Cannot execute");
					return -1;
				}
				redirection_flag = TRUE;
				output_redirection = TRUE;

			} else if (strcmp(line, "<") == STRING_EQUALS) { // if the symbol is '<' set up input redirection
				if (input_redirection) {// if more than one '<' symbol is found set error flag to clear the stream.
					perror("Ambiguous redirection. Cannot execute.");
					return -1;
				}
				redirection_flag = TRUE;
				input_redirection = TRUE;

				/* Checking for a valid out/input file. Using NULL instead of 0 to check 
				 * since 0 and NULL are equivalent
				 * Setting address pointer to coresponding file and incrementing line
				 * which is the stdin.
				 */
			} else if (output_redirection && outfile == NULL) {
				outfile = line;
				line += word_size + 1;
			} else if (input_redirection && infile == NULL) {
				infile = line;
				line += word_size + 1;
			}
			else { // add the commands to the commands array and increment line to point to the new word.
				word_count++;
				*commands++ = line;
				line += abs(word_size) + 1;// Using abs because of the negative sign from $  
			}

		}
	} while (word_size != EOF && word_size != ZERO_COMMANDS); // read until a line terminator is encountered

	if(word_size == EOF){
		word_count = EOF;

	}
	if(background_flag && word_count == ZERO_COMMANDS){
		perror("Background char '&' is in the wrong format or location");
		return -1;
	}
	if(output_redirection && outfile == NULL){
		perror("Redirection cannot output to a file.");
		return -1;
	}

	if(input_redirection && infile == NULL){
		perror("Redirection coudln't get input from input file.");
		return -1;
	}

	if(infile != NULL && word_count == 0){
		perror("No executable commands after the redirectioni from input file");
		return -1;

	}
	if(outfile != NULL && word_count == 0){
		perror("No executable commands after redirection to output file");
		return -1;
	}
	if(pipe_flag && pipe_flag >= word_count){
		perror("No process or executable commands after the pipeline");
		return -1;

	}	

	*commands = NULL; //null terminate string array of commands
	return word_count;
}

void exec_command(char *command, char **args){
	pid_t child_pid;
	int wait_status;
	fflush(stdout);// Flushing before a fork to ensure stdout & stdout buffers are clear for the child.
	fflush(stderr);

	if((child_pid = fork()) == FAIL){
		perror("Forked failed");
		return;	
	}
	/* Executes the child command */
	else if(child_pid == SUCCESS){
		run_child_command(command, args);
	}

	else {
		/* if background flag is not set then the shell waits
		 * for its childrne to die and any leftover zombies.
		 */
		if(!background_flag){
			do{
				wait_status = wait(NULL);

			}while(wait_status != child_pid);

		}
		else{
			printf("%s [%d]\n", command, child_pid);
			background_flag = FALSE;

		}

	}	

}

void run_child_command(char *command, char **args){
	int wait_status;
	pid_t child_pid;
	int redirection_return;	

	if(redirection_flag){

		if(redirection_return = set_up_redirection() < 0){
			perror("Redirection error");
			exit(-10);
		}
	}

	if(pipe_flag){
		int fds[2]; //file descriptors
		if(pipe(fds) == FAIL){
			perror("Pipe failed");
			exit(-11);
		}
		fflush(stdout); //Flush before fork everything.
		fflush(stderr);
		if((child_pid =fork()) == FAIL){
			perror("Fork failed inside of pipe code");
			exit(-12);	
		}

		/* The grandchild handles the redirection to the output to the of the pipe
		 * or write end of the pipe. One can think of it has the stdout of the pipe.
		 */
		else if(child_pid == SUCCESS){
			if(dup2(fds[1], STDOUT_FILENO) == FAIL){
				perror("Pipe output redirection failed using dup2");
				exit(-13);
			}

			close(fds[1]);
			close(fds[0]);
			if(execvp(command,args) == FAIL){
				perror("Execvp failed inside of pipeline code");
				exit(-14);
			}
		}
		else{
			if(!background_flag){
				do{
					wait_status = wait(NULL);
				}while(wait_status != child_pid);
				/*Redirecting child input to the output end of the pipe*/

				if(dup2(fds[0], STDIN_FILENO) == FAIL){
					perror("Pipe redirection failed from child's input to end of pipe");
					exit(-15);
				}

				close(fds[0]);
				close(fds[1]);

				if(execvp(args[pipe_flag + 1],args + (pipe_flag + 1)) == FAIL){
					perror("Exec failed with pipes");
					exit(-15);
				}	

			}
			else{
				printf("%s [%d]\n", command, child_pid);
				if(dup2(fds[0], STDIN_FILENO) == FAIL){
					perror("Pipe input redirection failed with background flag");
					exit(-16);
				}
				close(fds[0]);
				close(fds[1]);
				if(execvp(args[pipe_flag + 1], args  +(pipe_flag + 1)) == FAIL){
					perror("Execvp failed for pipes with background flag");
					exit(-16);
				}

			}	


		}


	}
	else{

		if(execvp(command, args) == FAIL){
			perror("execvp failed");	
			exit(-1);
		}
	}

}

int set_up_redirection(){
	int file_descriptor = 0;
	if(outfile != NULL){
		/*Open file for write only if it doesn't exists. It creates a file with
		 * read and write permissions as indicated b the flags passed to open
		 */
		if(file_exists(outfile) == SUCCESS){
			perror("File exists");
			return 	-1;

		}
		file_descriptor = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR, S_IWUSR);

		if(file_descriptor < SUCCESS){
			perror("Cannot open overwrite file");
			exit(-3);

		}
		/* Redirecting output from stdout to the open file*/

		if(dup2(file_descriptor, STDOUT_FILENO) < SUCCESS){
			perror("Output redirection with dup2 failed");
			exit(-4);
		}

		close(file_descriptor);


	}

	if(infile != NULL){
		file_descriptor = open(infile, O_RDONLY); //Only reading file
		if(file_descriptor < SUCCESS){
			perror("Cannot read input file");
			exit(-5);
		}
		if(dup2(file_descriptor, STDIN_FILENO) < SUCCESS){
			perror("Input file overwritten");
			exit(-6);
		}
		close(file_descriptor);
	}

	if(infile == NULL && background_flag){
		file_descriptor = open("/dev/null", O_RDONLY);
		if(file_descriptor < SUCCESS){
			perror("Cannot open file with background flag ");
			exit(-7);
		}
		if(dup2(file_descriptor, STDIN_FILENO) < SUCCESS){
			perror("input redirection failed with background flag");
			exit(-9);
		}
		close(file_descriptor);
	}
	return file_descriptor;

}

void clear_flags(){
	if(background_flag)
		background_flag = FALSE;
	if(outfile != NULL){
		outfile = NULL;
	}
	if(pipe_flag){
		pipe_flag = FALSE;
	}
	if(infile != NULL){
		infile = NULL;
	}

}
void sighandler(){


}
