#ifndef _SSHELL_H
#define _SSHELL_H
#include "sshellViewModel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void ViewStart();
void SplitInput(char* userInput, CommandAndArgument *listOfCommand);

#endif