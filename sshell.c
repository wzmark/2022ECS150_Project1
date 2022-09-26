#include "sshell.h"

int errorHandler(int errorType){
        switch(errorType){
                case 1:
                        //can not open output file
                        break;
                case 2:
                        //can not cd
                        break;
                case 3:
                        //invalid name
                        break;
                case 4:
                        //missing command
                        break;
                case 5:
                        //too much argument
                        break;
                case 6:
                        //missing output file
                        break;
                case 7:
                        //mislocated output redirection
                        break;
                default:
                        break;
                        
        }
        return 0;
}

int ExitHandler(char* userInput){
        if(strstr(userInput, "exit") != NULL){
                fprintf(stderr,"Bye...\n+ completed '%s' [0]\n", userInput);
		return 1;

        }else{
                return 0;
        }
}

int ExecuteDefinedCommand(CommandAndArgument *singleCommand){
        /*
        pid_t pid = fork();
        
        int ret;
        if(pid == 0){
                //printf("test \n");
                RedirectionOutput(singleCommand);
                ret = execvp(singleCommand->command, singleCommand->argument);
                
        }else if(pid > 0){
                int status;
                waitpid(pid, &status, 0);
                
        }else { 
                perror("fork");
                exit(1); 
        }
        */
        RedirectionOutput(singleCommand);
        int ret = execvp(singleCommand->command, singleCommand->argument);
        return ret;
}





void ExecutePipelineCommands(SshellInput *shell){
        pid_t pid;
        
        int fd[COMMAND_MAX_NUM][2];
        
        //fd = (int*)malloc((shell->numOfCommand - 1) * sizeof(int) * 2);
        

        for(int i = 0; i < shell->numOfCommand - 1; i++){
                if(pipe(fd[i]) < 0){
                        exit(1);
                }
        }


        for(int i = 0; i < shell->numOfCommand; i++){
                //pipe(fd);
                pid = fork();
                if(pid == 0){
                        //printf("test \n");
                        //int temp = *fd[(i - 1) * 2];
                        //printf("%d + %d", temp, fd[(i - 1) * 2]);
                        
                        if(i != 0){
                                dup2(*fd[(i - 1) * 2], STDIN_FILENO);
                        }else if(i != shell->numOfCommand - 1){
                                dup2(*fd[i * 2 + 1],STDOUT_FILENO);
                        }
                        for(int j = 0; j < COMMAND_MAX_NUM * 2; j++){
                                close(*fd[i]);
                        }

                        ExecuteCommand(&shell->listOfCommand[i]);
                        
                }
                else if(pid > 0){
                        int status;
                        waitpid(pid, &status, 0);
                        
                        
                }else { 
                        perror("fork");
                        exit(1); 
                }
                
        }
        for(int i = 0; i < (shell->numOfCommand - 1) * 2; i++){
                close(*fd[i]);
                
        }
        
        
}


void ErrorHandler(int statusCode){
        // 1 for too many argument
        // 2 for missing command
        switch(statusCode){
                case 1:

                        break;
                default:
                        break;
        }
}

void RedirectionOutput(CommandAndArgument *singleCommand){

        int fd;
        char fileName[PATH_MAX_LEN];
        if(singleCommand->isRedirect == 0){
                return;
        }else{
                int indexOfRedirectSign;
                for(int i = singleCommand->numOfArgument; i > 0; i--){
                        if(strcmp(singleCommand->argument[i], ">") == 0){
                                indexOfRedirectSign = i;
                        }
                }
                if(singleCommand->argument[indexOfRedirectSign + 1] == NULL){
                        //missing file name
                        return;
                }

                strcpy(fileName, singleCommand->argument[indexOfRedirectSign + 1]);
                singleCommand->argument[indexOfRedirectSign] = NULL;
                singleCommand->argument[indexOfRedirectSign + 1] = NULL;
                
        }
        

        fd = open(fileName, O_RDWR|O_CREAT);
        if(fd != -1){
                
                dup2(fd, STDOUT_FILENO);
                close(fd);
        }else{
                fprintf(stderr, "Error: cannot open output file\n");
        }
}

//if function find these command "pwd", "cd" return 1
int ExecuteBuildInCommand(CommandAndArgument *singleCommand){
        //RedirectionOutput(singleCommand);
        char *buffer = (char*)malloc(PATH_MAX_LEN);
        getcwd(buffer, PATH_MAX_LEN);
        if(strstr(singleCommand->command, "pwd") != NULL){
                //getcwd(buffer, PATH_MAX_LEN);
                fprintf(stdout, "%s\n", buffer);
                return 1;
        }else if(strstr(singleCommand->command, "cd") != NULL){
                if(singleCommand->numOfArgument == 0){
                        strcpy(singleCommand->argument[1], "HOME");
                }
                
                if(chdir(singleCommand->argument[1]) < 0){
                        fprintf(stderr, "Error: cannot cd into directory\n");
                }
                return 1;
        }
        return 0;
}

int SetVariable(VariableDictionary *listOfVariable, SshellInput *shell){
        
        for(int i = 0; i < shell->numOfCommand; i++){
                if(strcmp(shell->listOfCommand[i].command, "set") == 0 && shell->listOfCommand[i].numOfArgument == 2){
                        //retrieve number of variable for setting variable into list
                        int numberOfVariable = listOfVariable->numOfVariables;

                        listOfVariable->nameOfVariable[numberOfVariable] = (char*)malloc(VARIABLE_MAX_NUM  * sizeof(char));
                        listOfVariable->contentOfVariable[numberOfVariable] = (char*)malloc(VARIABLE_MAX_NUM  * sizeof(char));
                        strcpy(listOfVariable->nameOfVariable[numberOfVariable], shell->listOfCommand[i].argument[0]);
                        strcpy(listOfVariable->contentOfVariable[numberOfVariable], shell->listOfCommand[i].argument[1]);
                        numberOfVariable = numberOfVariable + 1;
                        listOfVariable->numOfVariables = numberOfVariable;
                }
        }
        return 0;
}

int FindVariable(VariableDictionary *listOfVariable, char *tempOfVarName){
        for(int i = 0; i < listOfVariable->numOfVariables; i++){
                if(strcmp(listOfVariable->nameOfVariable[i], tempOfVarName) == 0){
                        return i;
                }
        }
        return -1;
}

int RetrieveVariable(VariableDictionary *listOfVariable, SshellInput *shell){
        char *tempOfVarName;
        int tempOfPosition;
        for(int i = 0; i < shell->numOfCommand; i++){
                if(strstr(shell->listOfCommand[i].command, "$") != NULL){
                        tempOfVarName = strtok(shell->listOfCommand[i].command, "$");
                        tempOfPosition = FindVariable(listOfVariable, tempOfVarName);
                        if(tempOfPosition == -1){

                        }
                        //TODO: error handling in not finding position

                        strcpy(shell->listOfCommand[i].command, listOfVariable->contentOfVariable[tempOfPosition]);
                }

                for(int j = 0; j < shell->listOfCommand[i].numOfArgument; j++){
                        if(strstr(shell->listOfCommand[i].argument[j], "$") != NULL){
                                tempOfVarName = strtok(shell->listOfCommand[i].argument[j], "$");
                                tempOfPosition = FindVariable(listOfVariable, tempOfVarName);
                                if(tempOfPosition == -1){
                                
                                }
                                //TODO: error handling in not finding position

                                strcpy(shell->listOfCommand[i].argument[j], listOfVariable->contentOfVariable[tempOfPosition]);
                        }
                        
                }
        }
        return 0;
}


void ExecuteCommand(CommandAndArgument *singleCommand){
        //int ret;
        //pid_t pid;

        if(singleCommand->numOfArgument >= 1){
                char *newArgument = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
                strcpy(newArgument, singleCommand->command);
                
                char *tempArgument = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
                for(int i = 0; i < COMMAND_MAX_NUM; i++){
                        if(i < singleCommand->numOfArgument){

                                strcpy(tempArgument, singleCommand->argument[i]);
                                strcpy(singleCommand->argument[i], newArgument);
                                strcpy(newArgument, tempArgument);
                                
                        }else if(i == singleCommand->numOfArgument){
                                singleCommand->argument[i] = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
                                strcpy(singleCommand->argument[i], newArgument);
                                
                        }else{
                                singleCommand->argument[i] = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
                                singleCommand->argument[i] = NULL;
                        }  
                }
        }
        char *tempCommand = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
        strcpy(tempCommand, "/bin/");
        
        strcat(tempCommand, singleCommand->command);

        strcpy(singleCommand->command, tempCommand);
        
        if(ExecuteBuildInCommand(singleCommand) == 0){
                ExecuteDefinedCommand(singleCommand);
        }

        
        
}


void ViewStart(){
        char userInput[CMD_MAX_LEN];
        SshellInput shell;
        //CommandAndArgument listOfCommand[COMMAND_MAX_NUM];
        VariableDictionary listOfVariable;
        listOfVariable.numOfVariables = 0;
        printf("sshell$ ");
        while(fgets(userInput, CMD_MAX_LEN, stdin) != NULL){
                

                userInput[strcspn(userInput, "\n")] = 0;
                if(ExitHandler(userInput) == 1){
                        break;
                }
                SplitInput(userInput, shell.listOfCommand, &shell.numOfCommand);
                
                if(SetVariable(&listOfVariable, &shell) == 0){
                        RetrieveVariable(&listOfVariable, &shell);
                
                        ExecutePipelineCommands(&shell);
                }
                

                printf("sshell$ ");
        }
}

void SplitInput(char userInput[CMD_MAX_LEN], CommandAndArgument *listOfCommand, int *numOfCommand){
        //char *tempCommand;
        
        //char *tempArgument[ARGUMENT_MAX_LEN];
        //CommandAndArgument tempCommandAndArgument;
        char listOfTempString[COMMAND_MAX_NUM][COMMAND_MAX_LEN];
        char* splitString;
        //char* splitInput;
        char splitChar[2] = "|";
        //if find command set to 1, reset to zero after meet '|'
        int isFindCommand = 0;
        //if command is redirect, set to 1
        //int isRedirect = 0;

        int index = 0;
        splitString = strtok(userInput, splitChar);
        while(splitString != NULL){
                strcpy(listOfTempString[index],splitString);
                
                index++;
                splitString = strtok(NULL, splitChar);
        }
        *numOfCommand = index;

        
        *splitChar = ' ';
        int numOfArgument = 0;
        for(int i = 0; i < index; i++){
                if(RedirectionCommandHandler(listOfTempString[i]) == 1){
                        listOfCommand[i].isRedirect = 1;
                }else{
                        listOfCommand[i].isRedirect = 0;
                }
                splitString = strtok(listOfTempString[i], splitChar);
                
                while(splitString != NULL){
                        
                        if(isFindCommand == 0){
                                isFindCommand = 1; //find command
                                //tempCommand = (char*)malloc(COMMAND_MAX_LEN * sizeof(char));
                                listOfCommand[i].command = (char*)malloc(COMMAND_MAX_LEN * sizeof(char));
                                strcpy(listOfCommand[i].command, splitString);
                                
                                
                        }else{
                                
                                //tempArgument[i] = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
                                listOfCommand[i].argument[numOfArgument] = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
                                strcpy(listOfCommand[i].argument[numOfArgument], splitString);
                                numOfArgument++; 
                        }
                        
                        splitString = strtok(NULL, splitChar);
                }
                
                listOfCommand[i].numOfArgument = numOfArgument;
                isFindCommand = 0; //reset isFindCommand for next loop
                numOfArgument = 0;
                //tempCommand = NULL;
                
        }
        
}

int RedirectionCommandHandler(char *splitString){
        //handle special case xx>xxx xx> xx xx >xx
        //transform them to the xx > xx
        //return 1 if find > 

        //initialize var for front part and back part whether they exist
        char *finalString = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));     
        char *subString;
        if(strchr(splitString, '>') == NULL){
                //check whether can find >, if do not find return
                return 0;
        }else if(strlen(strchr(splitString, '>')) == 1){
                //find >, but not need to separate
                return 1;
        }else{
                subString = strtok(splitString, ">");
                int flag = 0; //first loop flag
                while(subString != NULL){
                        strcat(finalString, subString);
                        if(flag == 0){
                                strcat(finalString, " > ");
                                flag = 1;
                        }
                        subString = strtok(NULL, ">");
                }
                strcpy(splitString, finalString);
                
                return 1;
        }


        
        
}





int main(){
        
        
        ViewStart();
        
}
