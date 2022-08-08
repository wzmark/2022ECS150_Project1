#ifndef _SHELLMODEL_H
#define _SHELLMODEL_H


#include "array.h"

#define MAX_COMMAND 10
#define ARGUMENT_MAX_LEN 128
#define COMMAND_MAX_LEN 64

typedef struct{
    char *command;
    char *argument[];
    

}CommandAndArgument;

typedef struct{
    int numOfCommand;
    selfDefinedArray userInputArray;
    CommandAndArgument listOfCommandAndArgument[MAX_COMMAND];
    

}SshellInput;



void CheckNumOfCommand(SshellInput input);

#endif