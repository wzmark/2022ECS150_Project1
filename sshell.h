#ifndef _SSHELL_H
#define _SSHELL_H
#include "sshellModel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <fcntl.h>





void ViewStart();
int ExitHandler(char* userInput);
void ErrorHandler(int statusCode);
void ExecuteCommand(CommandAndArgument *singleCommand);
void ExecutePipelineCommands(SshellInput *shell);
int ExecuteDefinedCommand(CommandAndArgument *singleCommand);
void SplitInput(char* userInput, CommandAndArgument *listOfCommand, int *numOfCommand);
int RedirectionCommandHandler(char *splitString);
void RedirectionOutput(CommandAndArgument *singleCommand);
int ExecuteBuildInCommand(CommandAndArgument *singleCommand);
#endif