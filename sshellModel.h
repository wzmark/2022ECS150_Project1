#ifndef _SHELLMODEL_H
#define _SHELLMODEL_H




#include <unistd.h>
#include <sys/types.h>

#define ARGUMENT_MAX_LEN 32
#define ARGUMENT_MAX_NUM 10
#define CMD_MAX_LEN 512
#define COMMAND_MAX_LEN 32
#define COMMAND_MAX_NUM 5
#define PATH_MAX_LEN 256
#define PATH_MAX_NUM 32
#define VARIABLE_MAX_NUM 64


typedef struct{
        char *command; //store command
        char *argument[ARGUMENT_MAX_NUM]; //store command and argument
        int numOfArgument; // number of argument
        int isRedirect; //0 is not, 1 is redirect
        int isSuccess; //0 is not success, 1 success
        int isInverseRedirect; //0 is not, 1 is redirect

}CommandAndArgument;


typedef struct{
        char *DirectoryPath;//directory path in char* type
        struct Directory *nextDirectory; //next struct
}Directory;

//single linked list
typedef struct{
        int numOfDirectory; //number of directory store in the struct
        Directory *startDirectory; //start node of list
        Directory *endDirectory; //end node of the list
}DirectoryList;


typedef struct{
    CommandAndArgument listOfCommand[COMMAND_MAX_NUM]; //store split commnad and argument
    DirectoryList directoryStack; //stack of the directory
    int numOfCommand; //number of command
    char* savedCommand; //store the userInput
}SshellInput;

#endif