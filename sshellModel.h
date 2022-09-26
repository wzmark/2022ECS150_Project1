#ifndef _SHELLMODEL_H
#define _SHELLMODEL_H




#include <unistd.h>
#include <sys/types.h>

#define ARGUMENT_MAX_LEN 32
#define ARGUMENT_MAX_NUM 10
#define CMD_MAX_LEN 512
#define COMMAND_MAX_LEN 32
#define COMMAND_MAX_NUM 5
#define PATH_MAX_LEN 128
#define VARIABLE_MAX_NUM 64


typedef struct{
    char *command;
    char *argument[ARGUMENT_MAX_NUM];
    int numOfArgument;
    int isRedirect; //0 is not, 1 is redirect

}CommandAndArgument;

typedef struct{
    int numOfVariables;
    char *nameOfVariable[VARIABLE_MAX_NUM];
    char *contentOfVariable[VARIABLE_MAX_NUM];
}VariableDictionary;


typedef struct{
    CommandAndArgument listOfCommand[COMMAND_MAX_NUM];
    int numOfCommand;
}SshellInput;

#endif