#ifndef _SSHELL_H
#define _SSHELL_H
#include "sshellModel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <fcntl.h>
#include <sys/wait.h>




/*
* ViewStart - main interface start
* This function is the start of whole program, print out shell message and get 
* user input. The main data struct SshellInput shell will store in this function. 
* This data struct shell will pass to different function for different purpose
*
*/
void ViewStart();

/*
* InverseRedirect - inverse redirection of file
* @singleCommand: contain the command and argument 
*
* This function will open file and read the data from file. Firstly, this 
* function will relocate the position of the argument and add command in the 
* argument. Then, this function will open the file and use pipe to read the data
*
*/
void InverseRedirect(CommandAndArgument *singleCommand);

/*
* InverseRedirectionCommandHandler - deal with some special case
* @splitString: single combo of command and argument
*
* This function will detect of the symbol of < and add space in the both side of
* >. This function handles special case xx<xxx, xx< xx ,xx <xx and transform them
* to xx > xx.
*
* return: 1 if find > else return 0
*
*/
int InverseRedirectionCommandHandler(char *splitString);

/*
* PrintMessage - Print completed message
* @shell: data of whole shell
*
* This function will print out complete message base on the running result.
* By reading success status of each command, this function can use loop to form
* the status code of the completed message.
*
*/
void PrintMessage(SshellInput *shell);

/* 
* ExitHandler - check whether need to exit
* @userInput: original user input
* 
* This function will detect whether user input have keyword exit. If function 
* detects return 1 and exist in ViewStart function.
*
* return: 1 if find exit else 0
*
*/
int ExitHandler(char* userInput);

/*
* ErrorHandler - print error message
* statusCode: integer to determine error
*
* This function will print out the error messages. These messages include all 
* type of message. This function should be called when error occur
*
* return: 0 if print out error message
*
*/
int ErrorHandler(int statusCode);

/*
* ExecuteCommand - helper part of execute command
* @singleCommand: contain one command and argument
*
* This function just relocate the original argument and move command into 
* argument. call the ExecuteBuildInCommand at the end of function.
* 
*/
void ExecuteCommand(CommandAndArgument *singleCommand);

/*
* ExecutePipelineCommands - execute the pipeline and call execute function
* @shell: contain all data about shell
*
* This function will apply the pipeline. Function will determine whether have
* buildinfunction in the first place. set up the pipeline and run the pipeline
* If only one command execute, not need to setup and run pipeline.
* Parent need to wait all child process end.
* 
*/
void ExecutePipelineCommands(SshellInput *shell);

/*
* ExecuteDefinedCommand - execute normal command
* @singleCommand: contain one command and argument
*
* This function execute the set up combo of command and argument. 
*
* return: no return if success else return EXIT_FAILURE
*/
int ExecuteDefinedCommand(CommandAndArgument *singleCommand);

/*
* SplitInput - split user input into ideal struct
* @userInput: user input
* @listOfCommand: return this value, store struct of command and argument
* @numOfCommand: return this value, number of command 
*
* This function will split the user input into command and argument into data
* struct. Split user input into string list by the symbol of the pipeline, then
* split string into command and argument. Function will call two helper function
* to determine whether contain redirection or inverse redirection.
* 
*/
void SplitInput(char userInput[CMD_MAX_LEN], CommandAndArgument *listOfCommand, int *numOfCommand);

/*
* edirectionCommandHandler - deal with special case
* @splitString: single combo of command and argument
*
* This function will detect of the symbol of > and add space in the both side of
* >. This function handles special case xx>xxx, xx> xx ,xx >xx and transform them
* to xx > xx.
*
* return: 1 if find > else return 0
*
*/
int RedirectionCommandHandler(char *splitString);

/*
* RedirectionOutput - redirection of file
* @singleCommand: contain the command and argument 
*
* This function will open file and store the data into file. Firstly, this 
* function will relocate the position of the argument and add command in the 
* argument. Then, this function will open the file and use pipe to set up STDOUT
*
*/
void RedirectionOutput(CommandAndArgument *singleCommand);

/*
* ExecuteBuildInCommand - execute build-in command
* @singleCommand: command need to execute
* @shell: data of whole shell
*
* This function will execute commands, including pwd, cd, pushd, dirs, popd.
* In the first place, function will check whether singleCommand contains these 
* keyword. If not, directly return. Then, this function will use if to determine
* which key word need to execute.
*
* return: 0 if command not find or fail to execute else 1
*
*/
int ExecuteBuildInCommand(CommandAndArgument *singleCommand, SshellInput *shell);
#endif