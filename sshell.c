#include "sshell.h"

#define CMD_MAX_LEN 512
#define ARGUMENT_MAX_LEN 128
#define COMMAND_MAX_LEN 64
#define COMMAND_MAX_NUM 5



void ViewStart(){
        char userInput[CMD_MAX_LEN];
        CommandAndArgument listOfCommand[COMMAND_MAX_NUM];
        while(1){
                printf("sshell$ ");
                

                fgets(userInput, CMD_MAX_LEN, stdin);

                userInput[strcspn(userInput, "\n")] = 0;

                SplitInput(userInput, listOfCommand);
                printf("%s \n", listOfCommand);
                


        }
}

void SplitInput(char userInput[CMD_MAX_LEN], CommandAndArgument *listOfCommand){
        char *tempCommand;
        
        char *tempArgument[ARGUMENT_MAX_LEN];
        CommandAndArgument tempCommandAndArgument;
        char listOfTempString[COMMAND_MAX_NUM][COMMAND_MAX_LEN];
        char* splitString;
        char* splitInput;
        char splitChar[2] = "|";
        //if find command set to 1, reset to zero after meet '|'
        int isFindCommand = 0;

        int index = 0;
        splitString = strtok(userInput, splitChar);
        while(splitString != NULL){
                strcpy(listOfTempString[index],splitString);
                
                index++;
                splitString = strtok(NULL, splitChar);
        }
        
        *splitChar = ' ';
        int numOfArgument = 0;
        for(int i = 0; i < index; i++){
                splitString = strtok(listOfTempString[i], splitChar);
                
                while(splitString != NULL){
                        if(isFindCommand == 0){
                                isFindCommand = 1; //find command
                                tempCommand = (char*)malloc(COMMAND_MAX_LEN * sizeof(char));
                                listOfCommand[i].command = (char*)malloc(COMMAND_MAX_LEN * sizeof(char));
                                strcpy(listOfCommand[i].command, splitString);
                                
                        }else{
                                tempArgument[i] = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
                                listOfCommand[i].argument[numOfArgument] = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
                                strcpy(listOfCommand[i].argument[numOfArgument], splitString);
                                numOfArgument++; 
                        }
                        
                        splitString = strtok(NULL, splitChar);
                }
                //CommandAndArgument temp = malloc(COMMAND_MAX_LEN * sizeof(char) + ARGUMENT_MAX_LEN  * numOfArgument * sizeof(char));

                
                

                isFindCommand = 0; //reset isFindCommand for next loop
                numOfArgument = 0;
                tempCommand = NULL;
                
        }
        
}


int main(){
        ViewStart();
        
}
