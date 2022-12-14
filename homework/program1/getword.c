https://powcoder.com
代写代考加微信 powcoder
Assignment Project Exam Help
Add WeChat powcoder
https://powcoder.com
代写代考加微信 powcoder
Assignment Project Exam Help
Add WeChat powcoder
/*getword.c
 * - is a lexical analyzer that meets the requirements
 *	       of Program 0.
 *
 * Class Info- Bernardo Cervantes , John Carroll, CS570 ,
 *	       Due: 9/17/18 @ 8PM
 *		 
 * Synopsis  - Takes input from the keyboard (stdin) or from
 * 	       Data1/input# and writes to the Storage array 
 *	       and returns a int back to p0.
 *
 * Objective - Returns the correct input back to p0 so that 
 *             p0 will print out the correct int and string
 *	       that is identical to the same output in the 
 *	       p0 prompt and the what autograder produces.
 *             
 */

/* Include Files */
#include <stdio.h>
#include <stdlib.h>
#include "getword.h"
#define TRUE 1
#define FALSE 0 
/*
 * Getword is the principal method of getword.c
 * Takes in a char pointer that points to the STORAGE 
 * array to write chars to it based on the p1 prompt 
 * criteria. It return at int back to p1 that is the 
 * size of the string. p1 prints out the word which is the
 * STORAGE array and its size.
 *
 * 
 */

int backslash_flag ; //helps p2.c decipher metacharaceters from regular chars
int background_flag_from_getword ; //flag for & in p2.c
int tilde_flag; //use to indicate that getword has seen the tilde ~ metachar
int getword(char *w){
	int wordCount = 0; //Must initalize to zero or else you get garbage.
	int iochar = 0;
	int true = 0;      //bool for checking '$'
	//const char *name = "HOME";
	//char *value = getenv(name); //Sets home directory to value ; 
	//char *x = value;


	/* Stores intital char from STDIN/file */

	iochar = getchar(); 

	/* Skipping leading spaces. */

	while(iochar == ' '){
		iochar = getchar();
	}

	/* Check if EOF signal is in the beginning
	 * and puts null char in array (storage) to print
	 * an empty string. 
	 * Returns -255
	 */

	if(iochar == EOF){
		*w = '\0';    
		return -255;
	}

	/* Checks if newline is in the beginning of the input.
	 * Puts null char in array to print empty string.
	 * Returns 0
	 */

	if(iochar == '\n'){
			*w = '\0';
			return 0;
	}

	/*
	 * Checks if ";" is first char  
	 * in input. Puts null char in array to print
	 * null empty string.
	 * Return 0 
	 */ 

	if(iochar == ';'){
		*w = '\0';
		return 0;
	}


	/*
	 * Checks if "$" is first char  
	 * in input. Puts null char in array to print
	 * null empty string. Gets next char and checks 
	 * if it is EOF. Sets boolean true to 1 or true.
	 * Return 0 
	 */ 

	if(iochar == '$'){
		true = 1;
		iochar = getchar();
		if(iochar == EOF){
			*w = '\0';
			return 0;
		}
	} 

	/* Checks if the first char is ~. If it is then it 
	 *  writes to the STORAGE array with w* with the value:
	 *  home path directory which is dynamically generated with getenv().
	 *  *x points to the char array that contains the home directory 
	 *  and is used to write to w* which points to STORAGE array.
	 */

	if(iochar == '~' && true == 0){
		iochar = getchar();
		tilde_flag = TRUE;
	/*	while(*x != '\0'){
			*w = *x;
			w++;
			x++;
			wordCount++;
		}
	*/	
	}

	/*
	 *Checking for meta characters '<', '>','|. and '&'
	 * Special if statement case if next two char
	 * together are '<<'. If so it puts it in the STORAGE array
	 * and returns the size. Otherwise it does the same 
	 * for other single metacharacter.
	 *
	 * Using the system call ungetc to place the char back into stdin
	 * we do this because we find out that the char is not an additonal <
	 * to make << so we treat it as a single on the next call fo getword.
	 *
	 * 
	 */	
	if(iochar == '<' || iochar == '>' || iochar == '|' || iochar == '&'){


		if(iochar == '<'){
			*w = iochar;
			w++;	
			wordCount++;
			iochar = getchar();	

			if(iochar == '<'){
				*w = iochar;      		
				w++;
				wordCount++;
				*w = '\0';
				return wordCount;
			}
			else{
				iochar = ungetc(iochar , stdin);
				*w = '\0';
				return wordCount;
			}
		}




		/* Checking if & is at the EOF to set background_flag_from_getword
		 * to set the background_flag in p2.c 
		 * I know the code is the same in the if statments as the code
		 * 
		 *		*w = iochar;
		 *		w++;
		 *		wordCount++;
		 * after the if but doesn't work otherwise. 
		 * 
		 */

		if(iochar == '&'){
			*w = iochar;
			w++;
			wordCount++;
			iochar = getchar();
			if(iochar == '\n' || iochar == EOF){
				ungetc(iochar, stdin);
				*w = '\0';
				background_flag_from_getword = TRUE;
				return wordCount;
			}
			else {
				ungetc(iochar, stdin);
				*w = '\0';
				return wordCount;
			}

		}		
		*w = iochar;
		w++;
		wordCount++;
		*w = '\0';
		return wordCount;
	}




	/*
	 * Infinite for loop is used to get any remaining.  
	 * characters with exception chars accounted for such as
	 * EOF spaces, newlines and semicolons which are the same
	 * as the first checks but this time we return wordCount since
	 * these characters are not the first char in the words anymore.
	 * when the word is processed the storage array has been written
	 * with the word or an empty string based on some execptions
	 * and the wordcount is returned which stops the for loop.    
	 */ 

	for(;;){


		/* Checks to see if wordCount is greater than Storage (255).
		 * If so then it terminates the string and returns its
		 * current word count.
		 */

		if(wordCount == STORAGE - 1){
			iochar = ungetc(iochar, stdin);
			*w = '\0';
			return wordCount; 
		}


		/* Checks for EOF. If wordCount is anything but zero. It returns
		 * th word count. If not it returns -255. 
		 */

		if(iochar == EOF){
			if(wordCount < 0 || wordCount > 0){
				iochar = ungetc(iochar,stdin);
				*w = '\0';
				return wordCount;
			}

			return -255; 
		}

		/* Checks for space. If found, write null char to 
		 * array and 
		 * return wordCount.
		 */

		if(iochar == ' '){
			*w = '\0';
			return wordCount;
		}


		/* Checks for newline or semicolon. 
		 * If found, write null char to 
		 * array and 
		 * return wordCount.
		 */

		if(iochar == '\n'|| iochar == ';'){
			iochar = ungetc(iochar,stdin);
			*w = '\0';
			return wordCount;
		}

		/*
		 * Checks metacharacter again in but this time it for 
		 * chars not in the front. However it is handled the same 
		 * way. It puts the char back into stdin adds null char to the
		 * STORAGE array, and then returns the word size so the meta character 
		 * will be deal with the next program run.  
		 *
		 */

		if(iochar == '<' || iochar == '>' || iochar == '|' || iochar == '&'){

			ungetc(iochar, stdin);

			*w = '\0';
			return wordCount;
		}

		/*
		 *Checks for backslahes in input. Have to put two 
		 * two backslashes because C has one backslashs as a 
		 * special keyword.
		 */

		if(iochar  == '\\'){ 
			iochar = getchar();
			backslash_flag = TRUE;

			//if(iochar == '\n' || iochar == EOF){
			if(iochar == '\n'){
				iochar = getchar(); // newline is treated as a space for p2.c

			}

		}








		/* Writes Current Iochar To Array.
		 * Increments pointer to next array index. 
		 */

		*w = iochar;
		w++;

		/* Check if $ was at the beginning of a word.
		 * If true then wordCount is decremented 
		 *  and is negative. If not then the wordCount 
		 *  is incremented and is postive. 
		 */

		if(true == 1){
			wordCount--;
		}

		else{
			wordCount++;
		}
		/*Gets next char and repeats loop. */

		iochar = getchar();


		}

	}
