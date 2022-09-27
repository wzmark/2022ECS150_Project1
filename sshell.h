#ifndef _SSHELL_H
#define _SSHELL_H
#include "sshellModel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <fcntl.h>
#include <sys/wait.h>





void ViewStart();
int ExitHandler(char* userInput);
int ErrorHandler(int statusCode);
void ExecuteCommand(CommandAndArgument *singleCommand);
void ExecutePipelineCommands(SshellInput *shell);
int ExecuteDefinedCommand(CommandAndArgument *singleCommand);
void SplitInput(char userInput[CMD_MAX_LEN], CommandAndArgument *listOfCommand, int *numOfCommand);
int RedirectionCommandHandler(char *splitString);
void RedirectionOutput(CommandAndArgument *singleCommand);
int ExecuteBuildInCommand(CommandAndArgument *singleCommand);
#endif